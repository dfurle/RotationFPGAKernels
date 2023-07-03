#include "defines.h"
#include "parameters.h"
#include "myproject.h"


#include "rotationKernel.h"

void full_network(input_raw_t in1[N_INPUT_1_1], result_t out1[N_LAYER_8]){
  #ifdef REMOVE_FULL
  out1[0] = in1[0] - in1[1];
  #else
  #pragma HLS ARRAY_RESHAPE variable=in1 complete dim = 0
  #pragma HLS ARRAY_RESHAPE variable=out1 complete dim = 0
  // #pragma HLS PIPELINE

  #ifdef STREAM
  hls::stream<hls::vector<input_raw_t,N_INPUT_1_1>> in1_stream;
  hls::stream<hls::vector<layer1_t,N_INPUT_1_1>> rot_stream;
  hls::stream<hls::vector<result_t,N_LAYER_8>> out1_stream;
  #endif

  #ifdef MIDSTREAM
  hls::stream<hls::vector<layer1_t,N_INPUT_1_1>> rot_stream;
  #endif

  #if defined(STREAM) || defined(MIDSTREAM) || defined(VECTOR)
  hls::vector<input_raw_t,N_INPUT_1_1> in1_vec;
  hls::vector<result_t,N_LAYER_8> out1_vec;
  #pragma HLS ARRAY_RESHAPE variable = in1_vec complete dim = 0
  #pragma HLS ARRAY_RESHAPE variable = out1_vec complete dim = 0

  for(int i = 0; i < N_INPUT_1_1; i++){
    #pragma HLS unroll 
    in1_vec[i] = in1[i];
  }
  #endif

  #ifdef VECTOR
  hls::vector<layer1_t,N_INPUT_1_1> rot_vec;
  #endif
  #ifdef ARRAY
  layer1_t rot[N_INPUT_1_1];
  #pragma HLS ARRAY_RESHAPE variable=rot complete dim = 0
  #endif

  #ifdef STREAM
  in1_stream << in1_vec;
  #endif
  
  #ifdef STREAM
  rotationKernel(in1_stream, rot_stream);
  myproject(rot_stream, out1_stream);
  #endif
  #ifdef MIDSTREAM
  rotationKernel(in1_gm, rot_stream);
  myproject(rot_stream, out1_gm);
  #endif
  #ifdef VECTOR
  rotationKernel(in1_vec, rot_vec);
  myproject(rot_vec, out1_vec);
  #endif
  #ifdef ARRAY
  rotationKernel(in1, rot);
  myproject(rot, out1);
  #endif

  #ifdef STREAM
  out1_stream >> out1_vec;
  #endif

  #if defined(STREAM) || defined(MIDSTREAM) || defined(VECTOR)
  for(int i = 0; i < N_LAYER_8; i++){
    #pragma HLS unroll
    out1[i] = out1_vec[i];
  }
  #endif

  #endif
}


#ifdef STREAM
void myproject(hls::stream<hls::vector<layer1_t,N_INPUT_1_1>>& in1_stream, hls::stream<hls::vector<result_t,N_LAYER_8>>& out1_stream){
#endif
#ifdef VECTOR
void myproject(hls::vector<layer1_t,N_INPUT_1_1>& in1, hls::vector<result_t,N_LAYER_8>& out1){
#endif
#ifdef ARRAY
void myproject(layer1_t in1[N_INPUT_1_1], result_t out1[N_LAYER_8]){
#endif
#ifdef MIDSTREAM
void myproject(hls::stream<hls::vector<layer1_t,N_INPUT_1_1>>& in1_stream, result_t out1[N_LAYER_8]){
#endif
  // #pragma HLS INLINE
  #pragma HLS PIPELINE

  #ifdef REMOVE_NN
  for(int i = 0; i < N_LAYER_8; i++){
    #pragma HLS UNROLL
    out1[i] = in1[i];
  }
  return;
  #else
  

  #ifdef STREAM
  hls::vector<layer1_t,N_INPUT_1_1> in1;
  hls::vector<result_t,N_LAYER_8> out1;
  #endif

  #ifdef MIDSTREAM
  hls::vector<layer1_t,N_INPUT_1_1> in1;
  #endif

  #pragma HLS ARRAY_RESHAPE variable = in1 complete dim = 0
  #pragma HLS ARRAY_PARTITION variable = out1 complete dim = 0

  #if defined(STREAM) || defined(MIDSTREAM)
  in1_stream >> in1;
  #endif

  // hls-fpga-machine-learning insert layers
  layer2_t layer2_out[N_LAYER_2];
  #pragma HLS ARRAY_PARTITION variable = layer2_out complete dim = 0

  #if defined(STREAM) || defined(MIDSTREAM) || defined(VECTOR)
  // this seems to work, gives pointer to the data,
  //  atleast it outputs a value of 0.999 for the output...
  nnet::dense<layer1_t, layer2_t, config2>(in1.begin(), layer2_out, w2, b2); // dense
  #else
  nnet::dense<layer1_t, layer2_t, config2>(in1, layer2_out, w2, b2); // dense
  #endif

  layer3_t layer3_out[N_LAYER_2];
  #pragma HLS ARRAY_PARTITION variable = layer3_out complete dim = 0
  nnet::relu<layer2_t, layer3_t, relu_config3>(layer2_out, layer3_out); // dense_relu

  layer4_t layer4_out[N_LAYER_2];
  #pragma HLS ARRAY_PARTITION variable = layer4_out complete dim = 0
  nnet::normalize<layer3_t, layer4_t, config4>(layer3_out, layer4_out, s4, b4); // batch_normalization

  layer5_t layer5_out[N_LAYER_5];
  #pragma HLS ARRAY_PARTITION variable = layer5_out complete dim = 0
  nnet::dense<layer4_t, layer5_t, config5>(layer4_out, layer5_out, w5, b5); // dense_1

  layer6_t layer6_out[N_LAYER_5];
  #pragma HLS ARRAY_PARTITION variable = layer6_out complete dim = 0
  nnet::relu<layer5_t, layer6_t, relu_config6>(layer5_out, layer6_out); // dense_1_relu

  layer7_t layer7_out[N_LAYER_5];
  #pragma HLS ARRAY_PARTITION variable = layer7_out complete dim = 0
  nnet::normalize<layer6_t, layer7_t, config7>(layer6_out, layer7_out, s7, b7); // batch_normalization_1

  layer8_t layer8_out[N_LAYER_8];
  #pragma HLS ARRAY_PARTITION variable = layer8_out complete dim = 0
  nnet::dense<layer7_t, layer8_t, config8>(layer7_out, layer8_out, w8, b8); // dense_2

  result_t layer9_out[N_LAYER_8];
  #pragma HLS ARRAY_PARTITION variable = layer9_out complete dim = 0
  nnet::sigmoid<layer8_t, result_t, sigmoid_config9>(layer8_out, layer9_out); // dense_2_sigmoid

  // TODO: figure out how to quickly write to vector from array...
  // probably best to just loop/unroll? but im not sure
  // out1.data = layer9_out;
  out1[0] = layer9_out[0];

  #ifdef STREAM
  out1_stream << out1;
  #endif

  #endif
}
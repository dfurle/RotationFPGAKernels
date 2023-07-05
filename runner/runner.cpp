#include "globalDefines.h"

// #include "myproject.h"
#include "NNFakeOverlap.h"
#include "rotationKernel.h"

void runner(input_raw_t in1[N_INPUT_1_1], result_t out1[N_LAYER_8]){
// void runner(input_raw_t in1[N_INPUT_1_1 * NUM_TRACKS], result_t out1[N_LAYER_8 * NUM_TRACKS]){
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

  #if defined(STREAM) || defined(VECTOR)
  hls::vector<input_raw_t,N_INPUT_1_1> in1_vec;
  hls::vector<result_t,N_LAYER_8> out1_vec;
  #pragma HLS ARRAY_RESHAPE variable = in1_vec complete dim = 0
  #pragma HLS ARRAY_RESHAPE variable = out1_vec complete dim = 0
  #endif

  // for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
  //   #pragma HLS unroll

    #if defined(STREAM) || defined(VECTOR)
    for(int i = 0; i < N_INPUT_1_1; i++){
      #pragma HLS unroll 
      // in1_vec[i] = in1[i + (N_INPUT_1_1 * trkNum)];
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
    NN::NNFakeOverlap(rot_stream, out1_stream);
    // myproject(rot_stream, out1_stream);
    #endif
    #ifdef MIDSTREAM
    rotationKernel(in1, rot_stream);
    NN::NNFakeOverlap(rot_stream, out1);
    // myproject(rot_stream, out1_gm);
    #endif
    #ifdef VECTOR
    rotationKernel(in1_vec, rot_vec);
    NN::NNFakeOverlap(rot_vec, out1_vec);
    // myproject(rot_vec, out1_vec);
    #endif
    #ifdef ARRAY
    rotationKernel(in1, rot);
    NN::NNFakeOverlap(rot, out1);
    // myproject(rot, out1);
    #endif

    #ifdef STREAM
    out1_stream >> out1_vec;
    #endif

    #if defined(STREAM) || defined(VECTOR)
    for(int i = 0; i < N_LAYER_8; i++){
      #pragma HLS unroll
      out1[i] = out1_vec[i];
    }
    // for(int i = 0; i < N_LAYER_8; i++){
    //   #pragma HLS unroll
    //   out1[i + (trkNum)] = out1_vec[i];
    // }
    #endif

  // }
}
//
//    rfnoc-hls-neuralnet: Vivado HLS code for neural-net building blocks
//
//    Copyright (C) 2017 EJ Kreinar
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// #include <iostream>


#include "defines.h"
#include "parameters.h"
#include "myproject.h"

// #define SWSIM


#include "rotationKernel.h"

void full_network(input_raw_t in1_func[N_INPUT_1_1], result_t out1_func[N_LAYER_8]){
  // #pragma HLS INTERFACE m_axi port=in1_func bundle=aximm1
  // #pragma HLS INTERFACE m_axi port=out1_func bundle=aximm1

  // #pragma HLS ARRAY_RESHAPE variable = in1_func complete dim = 0
  // #pragma HLS ARRAY_RESHAPE variable = out1_func complete dim = 0
  // #pragma HLS PIPELINE

  hls::stream<hls::vector<input_raw_t,N_INPUT_1_1>> in1;
  hls::stream<hls::vector<layer1_t,N_INPUT_1_1>> rot;
  hls::stream<hls::vector<result_t,N_LAYER_8>> out1;

  hls::vector<input_raw_t,N_INPUT_1_1> inVec;
  hls::vector<result_t,N_LAYER_8> outVec;
  #pragma HLS ARRAY_RESHAPE variable = inVec complete dim = 0
  #pragma HLS ARRAY_RESHAPE variable = outVec complete dim = 0

  for(int i = 0; i < N_INPUT_1_1; i++){
    #pragma HLS unroll 
    inVec[i] = in1_func[i];
  }

  in1 << inVec;
  
  rotationKernel(in1, rot);
  myproject(rot, out1);

  out1 >> outVec;

  for(int i = 0; i < N_LAYER_8; i++){
    #pragma HLS unroll
    out1_func[i] = outVec[i];
  }

  // layer1_t rot[N_INPUT_1_1];
  // rotationKernel(in1_func, rot);
  // myproject(rot, out1_func);
}


void myproject(hls::stream<hls::vector<layer1_t,N_INPUT_1_1>>& in1_func, hls::stream<hls::vector<result_t,N_LAYER_8>>& out1_func){
// void myproject(hls::stream<layer1_t[N_INPUT_1_1]>& in1_func, hls::stream<result_t[N_LAYER_8]>& out1_func){
// void myproject(hls::stream<layer1_t>& in1_func, result_t* out1_func){
// void myproject(layer1_t in1[N_INPUT_1_1], result_t out1[N_LAYER_8]){
  // #pragma HLS INLINE

    // #pragma HLS INTERFACE m_axi port=in1_func bundle=aximm1
    // #pragma HLS INTERFACE m_axi port=out1_func bundle=aximm1

    hls::vector<layer1_t,N_INPUT_1_1> in1;
    hls::vector<result_t,N_LAYER_8> out1;
    #pragma HLS ARRAY_RESHAPE variable = in1 complete dim = 0
    #pragma HLS ARRAY_PARTITION variable = out1 complete dim = 0
    #pragma HLS PIPELINE
    in1_func >> in1;


    // for(int i = 0; i < N_INPUT_1_1; i++){
    //   #pragma HLS unroll factor=N_INPUT_1_1
    //   // in1[i] = in1_func[i];
    //   in1[i] = in1_func.read();
    // }




    // hls-fpga-machine-learning insert IO
    // #pragma HLS ARRAY_RESHAPE variable = input_in_raw complete dim = 0
    // #pragma HLS ARRAY_PARTITION variable = layer9_out complete dim = 0
    // #pragma HLS INTERFACE ap_vld port = input_in_raw,layer9_out
    // #pragma HLS PIPELINE

    // #ifndef __SYNTHESIS__
    // static bool loaded_weights = false;
    // if (!loaded_weights)
    // {
    //     // hls-fpga-machine-learning insert load weights
    //     nnet::load_weights_from_txt<dense_weight_t, 1536>(w2, "w2.txt");
    //     nnet::load_weights_from_txt<dense_bias_t, 64>(b2, "b2.txt");
    //     nnet::load_weights_from_txt<batch_normalization_scale_t, 64>(s4, "s4.txt");
    //     nnet::load_weights_from_txt<batch_normalization_bias_t, 64>(b4, "b4.txt");
    //     nnet::load_weights_from_txt<dense_1_weight_t, 2048>(w5, "w5.txt");
    //     nnet::load_weights_from_txt<dense_1_bias_t, 32>(b5, "b5.txt");
    //     nnet::load_weights_from_txt<batch_normalization_1_scale_t, 32>(s7, "s7.txt");
    //     nnet::load_weights_from_txt<batch_normalization_1_bias_t, 32>(b7, "b7.txt");
    //     nnet::load_weights_from_txt<dense_2_weight_t, 32>(w8, "w8.txt");
    //     nnet::load_weights_from_txt<dense_2_bias_t, 1>(b8, "b8.txt");
    //     loaded_weights = true;
    // }
    // #endif

    // ****************************************
    // NETWORK INSTANTIATION
    // ****************************************

    // rotate the kernel
    // layer1_t input_1[N_INPUT_1_1];
    // #pragma HLS ARRAY_PARTITION variable = input_1 complete dim = 0
    // rotationKernel(in1, input_1);
    // for(int i = 0; i < N_INPUT_1_1; i++){
    //   #pragma HLS unroll factor=N_INPUT_1_1
    //   input_1[i] = in1[i];
    // }


    // hls-fpga-machine-learning insert layers
    layer2_t layer2_out[N_LAYER_2];
    #pragma HLS ARRAY_PARTITION variable = layer2_out complete dim = 0

    // this seems to work, gives pointer to the data,
    //  atleast it outputs a value of 0.999 for the output...
    nnet::dense<layer1_t, layer2_t, config2>(in1.begin(), layer2_out, w2, b2); // dense


    // std::cout << "in1" << std::endl;
    // for(int i = 0; i < 24; i++){
    //   std::cout << float(in1[i]) << std::endl;
    // }
    // std::cout << "layer2" << std::endl;
    // for(int i = 0; i < 24; i++){
    //   std::cout << float(layer2_out[i]) << std::endl;
    // }

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

    #ifdef SWSIM
    std::cout << "sigmoid input: " << float(layer8_out[0]) << std::endl;
    #endif

    // std::cout << float(layer8_out[0]) << std::endl;

    // for(int i = 0; i < 1; i++){
    //   std::cout << float(*(in1.begin() + i)) << std::endl;
    // }

    // TODO: figure out how to quickly write to vector from array...
    // probably best to just loop/unroll? but im not sure
    // out1.data = layer9_out;
    out1[0] = layer9_out[0];

    out1_func << out1;

    // for(int i = 0; i < N_LAYER_8; i++){
    //   #pragma HLS unroll factor=N_LAYER_8
    //   out1_func.write(layer9_out[i]);
    //   // out1_func[i] = layer9_out[i];
    //   // out1_func[i] = layer8_out[i];
    // }

}
#ifndef DEFINES_H_
#define DEFINES_H_

#include "ap_fixed.h"
#include "ap_int.h"
#include "nnet_utils/nnet_types.h"
#include <cstddef>
#include <cstdio>

namespace NN
{

    // hls-fpga-machine-learning insert numbers
    #define N_INPUT_1_1 15
    #define N_LAYER_2 64
    #define N_LAYER_2 64
    #define N_LAYER_2 64
    #define N_LAYER_5 32
    #define N_LAYER_5 32
    #define N_LAYER_5 32
    #define N_LAYER_8 1
    #define N_LAYER_8 1

    // hls-fpga-machine-learning insert layer-precision
    typedef ap_fixed<24,8> input_t;
    typedef ap_fixed<24,8> model_default_t;
    typedef ap_fixed<24,8> layer2_t;
    typedef ap_fixed<24,8,AP_RND,AP_SAT> densenn0_dense_weight_t;
    typedef ap_fixed<24,2,AP_RND,AP_SAT> densenn0_dense_bias_t;
    typedef ap_uint<1> layer2_index;
    typedef ap_fixed<24,8,AP_RND,AP_SAT> layer3_t;
    typedef ap_fixed<18,8> DenseNN0_Dense_relu_table_t;
    typedef ap_fixed<24,8> layer4_t;
    typedef ap_fixed<24,8,AP_RND,AP_SAT> batch_normalization_scale_t;
    typedef ap_fixed<24,6,AP_RND,AP_SAT> batch_normalization_bias_t;
    typedef ap_fixed<24,8> layer5_t;
    typedef ap_fixed<24,3,AP_RND,AP_SAT> densenn2_dense_weight_t;
    typedef ap_fixed<24,1,AP_RND,AP_SAT> densenn2_dense_bias_t;
    typedef ap_uint<1> layer5_index;
    typedef ap_fixed<24,8,AP_RND,AP_SAT> layer6_t;
    typedef ap_fixed<18,8> DenseNN2_Dense_relu_table_t;
    typedef ap_fixed<24,8> layer7_t;
    typedef ap_fixed<24,8,AP_RND,AP_SAT> batch_normalization_1_scale_t;
    typedef ap_fixed<24,5,AP_RND,AP_SAT> batch_normalization_1_bias_t;
    typedef ap_fixed<24,8> layer8_t;
    typedef ap_fixed<24,3,AP_RND,AP_SAT> densenn4_dense_weight_t;
    typedef ap_fixed<24,1,AP_RND,AP_SAT> densenn4_dense_bias_t;
    typedef ap_uint<1> layer8_index;
    typedef ap_fixed<24,8,AP_RND,AP_SAT> result_t;
    typedef ap_fixed<18,8> DenseNN4_Dense_sigmoid_table_t;

}

#endif

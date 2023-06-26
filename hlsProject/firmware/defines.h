#ifndef DEFINES_H_
#define DEFINES_H_

#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_half.h"
#include "nnet_utils/nnet_types.h"
#include <cstddef>
#include <cstdio>

//hls-fpga-machine-learning insert numbers
#define N_INPUT_1_1 24
#define N_LAYER_2 64
#define N_LAYER_5 32
#define N_LAYER_8 1
// #define N_LAYER_8 24



//hls-fpga-machine-learning insert layer-precision
typedef ap_fixed<16,11> input_raw_t;

typedef ap_fixed<16,6> input_1_default_t;
typedef ap_fixed<16,6> input_t;
typedef ap_fixed<16,6> layer1_t;
typedef ap_fixed<16,6> model_default_t;
typedef ap_fixed<16,6> layer2_t;
typedef ap_fixed<16,6> dense_weight_t;
typedef ap_fixed<16,6> dense_bias_t;
typedef ap_fixed<16,6> dense_relu_default_t;
typedef ap_fixed<16,6,AP_RND,AP_SAT> layer3_t;
typedef ap_fixed<16,6> layer4_t;
typedef ap_fixed<16,6> batch_normalization_scale_t;
typedef ap_fixed<16,6> batch_normalization_bias_t;
typedef ap_fixed<16,6> layer5_t;
typedef ap_fixed<16,6> dense_1_weight_t;
typedef ap_fixed<16,6> dense_1_bias_t;
typedef ap_fixed<16,6> dense_1_relu_default_t;
typedef ap_fixed<16,6,AP_RND,AP_SAT> layer6_t;
typedef ap_fixed<16,6> layer7_t;
typedef ap_fixed<16,6> batch_normalization_1_scale_t;
typedef ap_fixed<16,6> batch_normalization_1_bias_t;
typedef ap_fixed<16,6> layer8_t;
typedef ap_fixed<16,6> dense_2_weight_t;
typedef ap_fixed<16,6> dense_2_bias_t;
typedef ap_fixed<16,6> dense_2_sigmoid_default_t;
typedef ap_fixed<16,6,AP_RND,AP_SAT> result_t;

#endif

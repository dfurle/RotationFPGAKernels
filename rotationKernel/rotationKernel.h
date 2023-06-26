#ifndef ROTATIONKERNER_H_
#define ROTATIONKERNER_H_

#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"

// #include "defines.h"

#define N_INPUT_1_1 24
typedef ap_fixed<16,11> input_raw_t;
typedef ap_fixed<16,6> layer1_t;

extern "C"{

// Prototype of top level function for C-synthesis
void rotationKernel(input_raw_t* in1, layer1_t* out1);
// void rotationKernel(input_raw_t* in1, hls::stream<layer1_t>& out1);
// void rotationKernel(
//     input_raw_t* input_1,
//     layer1_t* input_out
// );

}

#endif

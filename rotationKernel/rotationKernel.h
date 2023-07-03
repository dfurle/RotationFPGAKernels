#ifndef ROTATIONKERNER_H_
#define ROTATIONKERNER_H_

#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"
#include "hls_vector.h"

#include "defines.h"

#define N_INPUT_1_1 24
typedef ap_fixed<16,11> input_raw_t;
typedef ap_fixed<16,6> layer1_t;

#define STREAM
// #define VECTOR
// #define ARRAY
// #define MIDSTREAM

// #define REMOVE_ROTATION
// #define REMOVE_NN
// #define REMOVE_FULL

extern "C"{

// Prototype of top level function for C-synthesis
#ifdef STREAM
void rotationKernel(hls::stream<hls::vector<input_raw_t,N_INPUT_1_1>>& in1_stream, hls::stream<hls::vector<layer1_t,N_INPUT_1_1>>& out1_stream);
#endif
#ifdef VECTOR
void rotationKernel(hls::vector<input_raw_t,N_INPUT_1_1>& in1, hls::vector<layer1_t,N_INPUT_1_1>& out1);
#endif
#ifdef ARRAY
void rotationKernel(input_raw_t* in1, layer1_t* out1);
#endif
#ifdef MIDSTREAM
void rotationKernel(input_raw_t* in1, hls::stream<hls::vector<layer1_t,N_INPUT_1_1>>& out1_stream);
#endif
}

#endif

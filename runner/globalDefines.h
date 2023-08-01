#ifndef GLOBAL_DEFINES_H
#define GLOBAL_DEFINES_H
#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"
#ifndef HOST
#include "hls_vector.h"
#endif

#define N_INPUT_1_1 24
#define N_LAYER_8 1

#define NUM_TRACKS 10000
// #define NUM_TRACKS 100

typedef ap_fixed<16,11> input_raw_t;
typedef ap_fixed<16,6> layer1_t;
typedef ap_fixed<16,6> result_t;

extern "C"{

void runner(input_raw_t* in1_gmem, result_t* out1_gmem);

}

#endif

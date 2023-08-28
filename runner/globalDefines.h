#ifndef GLOBAL_DEFINES_H
#define GLOBAL_DEFINES_H
#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"
#ifndef HOST
#include "hls_vector.h"
#endif

// #define AP_INT_MAX_W 4096

#define N_INPUT_1_1 24
#define N_LAYER_8 1

// #define NUM_TRACKS 10000
#define NUM_TRACKS 2

typedef ap_fixed<16,11> input_raw_t;
typedef ap_fixed<16,6> layer1_t;
typedef ap_fixed<16,6> result_t;

#ifndef HOST
extern "C"{

// void runner(input_raw_t* in1_gmem, result_t* out1_gmem);
void runner(hls::stream<hls::vector<hls::vector<input_raw_t, N_INPUT_1_1>, NUM_TRACKS>> in1_stream, hls::stream<hls::vector<hls::vector<result_t, N_LAYER_8>, NUM_TRACKS>> out1_stream);
void readMemory(input_raw_t* in1_gmem, hls::stream<hls::vector<hls::vector<input_raw_t, N_INPUT_1_1>, NUM_TRACKS>> in1_stream);
void writeMemory(result_t* out1_gmem, hls::stream<hls::vector<hls::vector<result_t, N_LAYER_8>, NUM_TRACKS>> out1_stream);

}
#endif

#endif

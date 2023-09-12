#ifndef GLOBAL_DEFINES_H
#define GLOBAL_DEFINES_H
#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"
#ifndef HOST
#include "hls_vector.h"
#endif

#define NHITS 5
#define N_INPUT_1_1 15
#define N_LAYER_8 1

#define NUM_TRACKS 10000

typedef float input_raw_t;
typedef ap_fixed<24,8,AP_RND,AP_SAT> result_t;

extern "C"
{
    void runner(input_raw_t* in1_gmem, result_t* out1_gmem, int number_tracks);
}

#endif

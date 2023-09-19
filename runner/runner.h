#ifndef GLOBAL_DEFINES_H
#define GLOBAL_DEFINES_H
#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"

#include "defines.h"


extern "C"
{
    void runner(NN::input_raw_t* in1_gmem, NN::result_t* out1_gmem, int number_tracks);
}

#endif

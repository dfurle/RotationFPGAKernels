#ifndef GLOBAL_DEFINES_H
#define GLOBAL_DEFINES_H
#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"


#include "defines.h"

extern "C"
{
    void runner(NN::input_raw_t* in1_gmem, NN::result_t* out1_gmem);

    // void runner(const hls::vector<NN::input_raw_t, N_INPUT_1_1>* in1_gmem, hls::vector<NN::result_t, N_LAYER_8>* out1_gmem, int number_tracks);


}

#endif

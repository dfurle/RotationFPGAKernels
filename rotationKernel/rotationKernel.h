#ifndef ROTATIONKERNER_H_
#define ROTATIONKERNER_H_

#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"
#include "hls_vector.h"

#include "defines.h"

#include "globalDefines.h"

extern "C"
{

    // Prototype of top level function for C-synthesis
    void rotationKernel(hls::vector<input_raw_t,N_INPUT_1_1>& in1, hls::vector<NN::input_t,N_INPUT_1_1>& out1);
}

#endif

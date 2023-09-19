#ifndef ROTATIONKERNER_H_
#define ROTATIONKERNER_H_

#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"

#ifndef HOST
#include "hls_vector.h"
#endif

#include "defines.h"


extern "C"
{
    // Prototype of top level function for C-synthesis
    void rotationKernel(hls::vector<NN::input_raw_t,N_INPUT_1_1>& in1, hls::vector<NN::input_t,N_INPUT_1_1>& out1);
}

#endif

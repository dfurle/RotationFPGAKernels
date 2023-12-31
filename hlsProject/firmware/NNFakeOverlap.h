#ifndef NNFAKEOVERLAP_H_
#define NNFAKEOVERLAP_H_

#include "ap_fixed.h"
#include "ap_int.h"
#include "hls_stream.h"

#ifndef HOST
    #include "hls_vector.h"
#endif

#include "defines.h"

namespace NN
{
    extern "C"
    {
        void NNFakeOverlap(hls::vector<input_t,N_INPUT_1_1>& in1, hls::vector<result_t,N_LAYER_8>& out1);
    }
}

#endif

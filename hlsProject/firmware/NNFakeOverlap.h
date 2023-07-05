#ifndef NNFAKEOVERLAP_H_
#define NNFAKEOVERLAP_H_

#include "ap_fixed.h"
#include "ap_int.h"
#include "hls_stream.h"

#include "defines.h"

#include "globalDefines.h"

namespace NN{

extern "C"{

#ifdef STREAM
void NNFakeOverlap(hls::stream<hls::vector<layer1_t,N_INPUT_1_1>>& in1_stream, hls::stream<hls::vector<result_t,N_LAYER_8>>& out1_stream);
#endif
#ifdef VECTOR
void NNFakeOverlap(hls::vector<layer1_t,N_INPUT_1_1>& in1, hls::vector<result_t,N_LAYER_8>& out1);
#endif
#ifdef ARRAY
void NNFakeOverlap(layer1_t in1[N_INPUT_1_1], result_t out1[N_LAYER_8]);
#endif
#ifdef MIDSTREAM
void NNFakeOverlap(hls::stream<hls::vector<layer1_t,N_INPUT_1_1>>& in1_stream, result_t out1[N_LAYER_8]);
#endif

}

}

#endif

#include "rotationKernel.h"
#include <hls_math.h>
#include <iostream>

typedef float internalType;
// typedef ap_fixed<32,10> internalType;


void rotationKernel(hls::vector<input_raw_t,N_INPUT_1_1>& in1, hls::vector<NN::input_t,N_INPUT_1_1>& out1){
  #pragma HLS INLINE off
  #pragma HLS PIPELINE
  // #pragma HLS DATAFLOW

  // #pragma HLS ARRAY_RESHAPE variable = in1 complete dim = 0
  // #pragma HLS ARRAY_RESHAPE variable = out1 complete dim = 0

  // const internalType _XScale = 0.00094;
  // const internalType _YScale = 0.00094;
  const internalType _ZScale = 0.00033;

  // const internalType overD = 0.00344;

  const internalType _combinedXYScale = 0.0000032336;


  bool flipZ = (in1[0] < 0);

  // There are NHITS * 3 information [0, NHit-1] is X, [Nhit, 2*NHits-1] is Y and [2*NHit, 3*NHits-1] is Z
  internalType cos_angle = in1[0] * _combinedXYScale; 
  internalType sin_angle = -1 * in1[NHITS] * _combinedXYScale;

XYRotation:
  for (int i = 0; i < NHITS; i++){
    #pragma HLS unroll

    int yIndex = i + NHITS;
    // rotating/flipping

    out1[i]      = (in1[i] * cos_angle - in1[yIndex] * sin_angle);
    out1[yIndex] = (in1[i] * sin_angle + in1[yIndex] * cos_angle);

  }
ZFlipScale:
  for (int i = 0; i < NHITS; i++){
    #pragma HLS unroll

    int index = i + 2*NHITS;
    // rotating/flipping
    if (flipZ)
    {
      out1[index] = -1 * in1[index] * _ZScale;
    }
    else
    {
      out1[index] = in1[index] * _ZScale;
    }
  }
}

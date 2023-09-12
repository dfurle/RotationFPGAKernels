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

  const internalType _XScale = 0.00094;
  const internalType _YScale = 0.00094;
  const internalType _ZScale = 0.00033;

  const internalType overD = 0.00344;

  bool flipZ = (in1[0] < 0);

  // There are NHITS * 3 information [0, NHit-1] is X, [Nhit, 2*NHits-1] is Y and [2*NHit, 3*NHits-1] is Z
  internalType cos_angle = in1[0]*overD; 
  internalType sin_angle = -1 * in1[NHITS]*overD;

  std::cout<<"cos_angle: "<<cos_angle<<" sin_angle: "<<sin_angle<<std::endl;

  hls::vector<internalType,N_INPUT_1_1> mid;

XYRotation:
  for (int i = 0; i < NHITS; i++){
    #pragma HLS unroll
    // #pragma HLS unroll factor = 8 skip_exit_check
    // #pragma HLS unroll factor = 8 region skip_exit_check

    int yIndex = i + NHITS;
    // rotating/flipping

    // rotating/flipping
    // out1[i] =  in1[i]*_XScale;
    // out1[yIndex] = in1[yIndex]*_YScale;

    out1[i]      = (in1[i] * cos_angle - in1[yIndex] * sin_angle) * _XScale;
    out1[yIndex] = (in1[i] * sin_angle + in1[yIndex] * cos_angle) * _YScale;

    mid[i]      = (in1[i] * cos_angle - in1[yIndex] * sin_angle) * _XScale;
    mid[yIndex] = (in1[i] * sin_angle + in1[yIndex] * cos_angle) * _YScale;

  }
ZFlipScale:
  for (int i = 0; i < NHITS; i++){
    #pragma HLS unroll
    // #pragma HLS unroll factor = 8 skip_exit_check
    // #pragma HLS unroll factor = 8 region skip_exit_check
    int index = i + 2*NHITS;
    // rotating/flipping
    // out1[index] = in1[index]*_ZScale;
    if (flipZ){
      out1[index] = -1 * in1[index] * _ZScale;
    }else{
      out1[index] = in1[index] * _ZScale;
    }
    mid[index] = in1[index] * _ZScale;
  }

    std::cout<<"Mid: ";
    for(int i = 0; i < N_INPUT_1_1; i++)
    {
      float x = mid[i];
      std::cout<<x<<" ";
    }
    std::cout<<std::endl;

}

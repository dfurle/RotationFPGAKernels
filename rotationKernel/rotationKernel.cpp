#include "rotationKernel.h"
#include <hls_math.h>

typedef ap_fixed<32,10> internalType;


void rotationKernel(hls::vector<input_raw_t,N_INPUT_1_1>& in1, hls::vector<layer1_t,N_INPUT_1_1>& out1){
  // #pragma HLS INLINE
  #pragma HLS PIPELINE
  // #pragma HLS DATAFLOW

  #pragma HLS ARRAY_RESHAPE variable = in1 complete dim = 0
  #pragma HLS ARRAY_RESHAPE variable = out1 complete dim = 0

  const internalType pi = 3.14;
  const internalType _XScale = 0.000936329588;
  const internalType _YScale = 0.000936329588;
  const internalType _ZScale = 0.0003305238804;

  bool flipZ = (in1[0] > 0);
  internalType div = in1[0] / in1[8];
  internalType rotateAngle = hls::atan(div);
  if (in1[7] < 0)
    rotateAngle = rotateAngle + pi;

  internalType cos_angle = hls::cos(rotateAngle);
  internalType sin_angle = hls::sin(rotateAngle);

XYRotation:
  for (int i = 0; i < 8; i++){
    #pragma HLS unroll
    // #pragma HLS unroll factor = 8 skip_exit_check
    // #pragma HLS unroll factor = 8 region skip_exit_check

    int yIndex = i + 8;
    // rotating/flipping

    // rotating/flipping
    // out1[i] =  in1[i]*_XScale;
    // out1[yIndex] = in1[yIndex]*_YScale;

    out1[i]      = (in1[i] * cos_angle - in1[yIndex] * sin_angle) * _XScale;
    out1[yIndex] = (in1[i] * sin_angle + in1[yIndex] * cos_angle) * _YScale;
  }
ZFlipScale:
  for (int i = 0; i < 8; i++){
    #pragma HLS unroll
    // #pragma HLS unroll factor = 8 skip_exit_check
    // #pragma HLS unroll factor = 8 region skip_exit_check
    int index = i + 16;
    // rotating/flipping
    // out1[index] = in1[index]*_ZScale;
    if (flipZ){
      out1[index] = -1 * in1[index] * _ZScale;
    }else{
      out1[index] = in1[index] * _ZScale;
    }
  }
}

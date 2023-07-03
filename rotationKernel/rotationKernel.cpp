#include "rotationKernel.h"
#include <hls_math.h>


typedef ap_fixed<32,10> internalType;


#ifdef STREAM
void rotationKernel(hls::stream<hls::vector<input_raw_t,N_INPUT_1_1>>& in1_stream, hls::stream<hls::vector<layer1_t,N_INPUT_1_1>>& out1_stream){
#endif
#ifdef VECTOR
void rotationKernel(hls::vector<input_raw_t,N_INPUT_1_1>& in1, hls::vector<layer1_t,N_INPUT_1_1>& out1){
#endif
#ifdef ARRAY
void rotationKernel(input_raw_t in1[N_INPUT_1_1], layer1_t out1[N_INPUT_1_1]){
#endif
#ifdef MIDSTREAM
void rotationKernel(input_raw_t in1[N_INPUT_1_1], hls::stream<hls::vector<layer1_t,N_INPUT_1_1>>& out1_stream){
#endif
  // #pragma HLS INLINE
  #pragma HLS PIPELINE
  // #pragma HLS DATAFLOW

  #ifdef REMOVE_ROTATION
  for(int i = 0; i < N_INPUT_1_1; i++){
    #pragma HLS UNROLL
    out1[i] = in1[i];
  }
  #else

  #ifdef STREAM
  hls::vector<input_raw_t,N_INPUT_1_1> in1;
  hls::vector<layer1_t,N_INPUT_1_1> out1;
  #endif

  #ifdef MIDSTREAM
  hls::vector<layer1_t,N_INPUT_1_1> out1;
  #endif

  #pragma HLS ARRAY_RESHAPE variable = in1 complete dim = 0
  #pragma HLS ARRAY_PARTITION variable = out1 complete dim = 0

  #ifdef STREAM
  in1_stream >> in1;
  #endif

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

  #if defined(STREAM) || defined(MIDSTREAM)
  out1_stream << out1;
  #endif

  #endif
}

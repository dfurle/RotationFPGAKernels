#include "rotationKernel.h"
#include <hls_math.h>

typedef ap_fixed<32,10> internalType;
// //typedef ap_fixed<16,6> internalType;

// const internalType pi2 = 1.57;
// const internalType t2 = 2.;
// const internalType t3 = 3.;
// const internalType t4 = 4.;
// const internalType t5 = 5.;

// internalType atanFunc(internalType x){
// 	if(x > 1)
// 		return pi2 - 1/x;
// 	if(x < -1)
// 		return -pi2 - 1/x;
// 	return x - x*x*x/t3 + x*x*x*x*x/t5;
// }
// internalType sinFunc(internalType x){
// 	return x - x*x*x/t3 + x*x*x*x*x/t5;
// }

// internalType cosFunc(internalType x){
// 	return 1 - x*x/t2 + x*x*x*x/t4;
// }
// void rotationKernel(input_raw_t in1[N_INPUT_1_1], layer1_t out1[N_INPUT_1_1]){

// }

void rotationKernel(input_raw_t in1[N_INPUT_1_1], layer1_t out1[N_INPUT_1_1]){
// void rotationKernel(input_raw_t in1[N_INPUT_1_1], hls::stream<layer1_t>& out1){
    // #pragma HLS INTERFACE m_axi port=in1 bundle=aximm1
    // #pragma HLS INTERFACE m_axi port=out1 bundle=aximm1

    input_raw_t input_1[N_INPUT_1_1];
    layer1_t input_out[N_INPUT_1_1];

    for(int i = 0; i < N_INPUT_1_1; i++){
      #pragma HLS unroll factor=N_INPUT_1_1
      input_1[i] = in1[i];
    }

    // hls-fpga-machine-learning insert IO
    // #pragma HLS ARRAY_RESHAPE variable = input_1 complete dim = 0
    // #pragma HLS ARRAY_PARTITION variable = input_out complete dim = 0
    // #pragma HLS INTERFACE ap_vld port = input_1
    // #pragma HLS dataflow

    const internalType pi = 3.14;
    const internalType _XScale = 0.000936329588;
    const internalType _YScale = 0.000936329588;
    const internalType _ZScale = 0.0003305238804;

    bool flipZ = (input_1[0] > 0);
    internalType rotateAngle = hls::atan(input_1[0] / input_1[8]);
    // internalType rotateAngle = atanFunc(input_1[0] / input_1[8]);
    if (input_1[7] < 0)
        rotateAngle = rotateAngle + pi;

    internalType cos_angle = hls::cos(rotateAngle);
    internalType sin_angle = hls::sin(rotateAngle);
    // internalType cos_angle = cosFunc(rotateAngle);
    // internalType sin_angle = sinFunc(rotateAngle);

XYRotation:
    for (int i = 0; i < 8; i++)
    {
        #pragma HLS unroll factor = 8 skip_exit_check
		// #pragma HLS unroll factor = 8 region skip_exit_check

        int yIndex = i + 8;
        // rotating/flipping

        // rotating/flipping
        // input_out[i] =  input_1[i]*_XScale;
        // input_out[yIndex] = input_1[yIndex]*_YScale;

        input_out[i]      = (input_1[i] * cos_angle - input_1[yIndex] * sin_angle) * _XScale;
        input_out[yIndex] = (input_1[i] * sin_angle + input_1[yIndex] * cos_angle) * _YScale;
    }
ZFlipScale:
    for (int i = 0; i < 8; i++)
    {
        #pragma HLS unroll factor = 8 skip_exit_check
        // #pragma HLS unroll factor = 8 region skip_exit_check
        int index = i + 16;
        // rotating/flipping
        // input_out[index] = input_1[index]*_ZScale;
        if (flipZ)
        {
            input_out[index] = -1 * input_1[index] * _ZScale;
        }
        else
        {
            input_out[index] = input_1[index] * _ZScale;
        }
    }


    for(int i = 0; i < N_INPUT_1_1; i++){
      #pragma HLS unroll factor=N_INPUT_1_1
      out1[i] = input_out[i];
      // out1.write(input_out[i]);
    }
}

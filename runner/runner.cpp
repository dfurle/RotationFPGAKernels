#include "globalDefines.h"
#include <iostream>

#include "NNFakeOverlap.h"
#include "rotationKernel.h"


extern "C" {

  void runner(input_raw_t* in1_gmem, result_t* out1_gmem, int number_tracks)
  {
    // #pragma HLS INTERFACE s_axilite port=return bundle=runner_port
    // #pragma HLS INTERFACE m_axi port=in1_gmem  offset=slave bundle=input
    // #pragma HLS INTERFACE m_axi port=out1_gmem offset=slave bundle=output
    // #pragma HLS INTERFACE s_axilite port=in1_gmem
    // #pragma HLS INTERFACE s_axilite port=out1_gmem


    // #pragma HLS ARRAY_RESHAPE variable=in1 complete dim = 0
    // #pragma HLS ARRAY_RESHAPE variable=rot complete dim = 0
    // #pragma HLS ARRAY_RESHAPE variable=out1 complete dim = 0


    MAIN_RUNNER:
    for(int trkNum = 0; trkNum < number_tracks; trkNum++)
    {
      // #pragma HLS PIPELINE off
      #pragma HLS PIPELINE II=3
      hls::vector<input_raw_t,N_INPUT_1_1> in1_loc;

      // input_raw_t buff[24];
      // memcpy(buff, (const input_raw_t*) in1_gmem, 24 * sizeof(input_raw_t));

      READ_DATA:
      for(int i = 0; i < N_INPUT_1_1; i++){
        #pragma HLS unroll
        in1_loc[i] = in1_gmem[i + (N_INPUT_1_1 * trkNum)];
        // in1_loc[i] = buff[i];
      }

      hls::vector<NN::input_t,N_INPUT_1_1> rot_loc;
      rotationKernel(in1_loc, rot_loc);

      hls::vector<result_t,N_LAYER_8> out1_loc;
      NN::NNFakeOverlap(rot_loc, out1_loc);
      // out1_loc[0] = in1_loc[0];

      WRITE_DATA:
      for(int i = 0; i < N_LAYER_8; i++){
        #pragma HLS unroll
        out1_gmem[i + (N_LAYER_8 * trkNum)] = out1_loc[i];
      }
    }


  }
}



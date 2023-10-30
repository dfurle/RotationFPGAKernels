#include "runner.h"
#include <iostream>


#include "firmware/NNFakeOverlap.h"
#include "rotationKernel.h"


extern "C" {

  void runner(NN::input_raw_t* in1_gmem, NN::result_t* out1_gmem)
  {
    #pragma HLS INTERFACE m_axi port=in1_gmem  offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=out1_gmem offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=in1_gmem   bundle=control
    #pragma HLS INTERFACE s_axilite port=out1_gmem  bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    // For 
    #pragma HLS DATAFLOW


    // Read the data into local buffers
    NN::input_raw_t in_bigbuf[INPUTTRACKSIZE * N_INPUT_1_1];
    NN::result_t out_bigbuf[INPUTTRACKSIZE * N_LAYER_8 ];
    READ_DATA:
    //getting data from DDR
    for (int i = 0; i < INPUTTRACKSIZE * N_INPUT_1_1; i++) 
    {
      // #pragma HLS PIPELINE II=2
      in_bigbuf[i] = in1_gmem[i];
    }
    // #pragma HLS ARRAY_RESHAPE   variable=in_bigbuf  complete dim=2
    // #pragma HLS ARRAY_RESHAPE   variable=out_bigbuf complete dim=2

    MAIN_RUNNER:
    for (int j = 0; j < INPUTTRACKSIZE; j++) 
    {  
      // #pragma HLS PIPELINE off
      // #pragma HLS PIPELINE II=10
      hls::vector<NN::input_raw_t,N_INPUT_1_1> in1_loc;

      REORGANIZE_DATA:
      for(int i = 0; i < N_INPUT_1_1; i++)
      {
        #pragma HLS unroll
        in1_loc[i] = in_bigbuf[i + (N_INPUT_1_1 * j)];
      }
      
      RUNNING_KERNEL:
      hls::vector<NN::input_t,N_INPUT_1_1> rot_loc;
      rotationKernel(in1_loc, rot_loc);

      hls::vector<NN::result_t,N_LAYER_8> out1_loc;
      NN::NNFakeOverlap(rot_loc, out1_loc);

      SAVE_DATA:
      for(int i = 0; i < N_LAYER_8; i++)
      {
        #pragma HLS unroll
        out_bigbuf[i + (N_LAYER_8 * j)] = out1_loc[i];
      }
    }


    WRITE_DATA:
    // Save the data to DDR
    for (int i = 0; i < INPUTTRACKSIZE * N_LAYER_8; i++) 
    {
      out1_gmem[i] = out_bigbuf[i];
    }
  }
}



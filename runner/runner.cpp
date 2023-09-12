#include "globalDefines.h"
#include <iostream>

// #include "myproject.h"


#include "NNFakeOverlap.h"
#include "rotationKernel.h"

// #include <string.h>

// void full_network(input_raw_t in1[N_INPUT_1_1], result_t out1[N_LAYER_8]);
extern "C" {

// void runner(input_raw_t in1_gmem[N_INPUT_1_1 * NUM_TRACKS], result_t out1_gmem[N_LAYER_8 * NUM_TRACKS]){
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

  // out1_gmem[0] = in1_gmem[0];

  MAIN_RUNNER:
  // for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
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

    for(int i = 0; i < N_INPUT_1_1; i++)
    {
      float x = rot_loc[i];
      std::cout<<x<<" ";
    }
    std::cout<<std::endl;

    hls::vector<result_t,N_LAYER_8> out1_loc;
    NN::NNFakeOverlap(rot_loc, out1_loc);
    // out1_loc[0] = in1_loc[0];

    WRITE_DATA:
    for(int i = 0; i < N_LAYER_8; i++){
      #pragma HLS unroll
      out1_gmem[i + (N_LAYER_8 * trkNum)] = out1_loc[i];
    }
  }

  //  other


  // hls::vector<hls::vector<input_raw_t,N_INPUT_1_1>, NUM_TRACKS> in1;
  // hls::vector<hls::vector<input_t,N_INPUT_1_1>, NUM_TRACKS> rot;
  // hls::vector<hls::vector<result_t,N_LAYER_8>, NUM_TRACKS> out1;

  // READ_DATA:
  // for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
  //   for(int i = 0; i < N_INPUT_1_1; i++){
  //     // #pragma HLS unroll
  //     in1[trkNum][i] = in1_gmem[i + (N_INPUT_1_1 * trkNum)];
  //   }
  // }

  // ROTATION:
  // for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
  //   // #pragma HLS unroll
  //   // #pragma HLS PIPELINE 
  //   hls::vector<input_raw_t,N_INPUT_1_1> in1_loc;
  //   hls::vector<input_t,N_INPUT_1_1> rot_loc;
  //   in1_loc = in1[trkNum];
  //   rotationKernel(in1_loc, rot_loc);
  //   rot[trkNum] = rot_loc;
  // }

  // NNKERNEL:
  // for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
  //   // #pragma HLS PIPELINE 
  //   hls::vector<input_t,N_INPUT_1_1> rot_loc;
  //   hls::vector<result_t,N_LAYER_8> out1_loc;
  //   rot_loc = rot[trkNum];
  //   NN::NNFakeOverlap(rot_loc, out1_loc);
  //   out1[trkNum] = out1_loc;
  // }

  // WRITE_DATA:
  // for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
  //   for(int i = 0; i < N_LAYER_8; i++){
  //     // #pragma HLS unroll
  //     out1_gmem[i + (N_LAYER_8 * trkNum)] = out1[trkNum][i];
  //   }
  // }

}
}


// void full_network(input_raw_t in1[N_INPUT_1_1], result_t out1[N_LAYER_8]){
// // void runner(input_raw_t in1[N_INPUT_1_1 * NUM_TRACKS], result_t out1[N_LAYER_8 * NUM_TRACKS]){

//   #pragma HLS ARRAY_RESHAPE variable=in1 complete dim = 0
//   #pragma HLS ARRAY_RESHAPE variable=out1 complete dim = 0
//   // #pragma HLS PIPELINE

//   hls::vector<input_raw_t,N_INPUT_1_1> in1_vec;
//   hls::vector<result_t,N_LAYER_8> out1_vec;
//   #pragma HLS ARRAY_RESHAPE variable = in1_vec complete dim = 0
//   #pragma HLS ARRAY_RESHAPE variable = out1_vec complete dim = 0

//   // for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
//   //   #pragma HLS unroll

//     for(int i = 0; i < N_INPUT_1_1; i++){
//       #pragma HLS unroll 
//       // in1_vec[i] = in1[i + (N_INPUT_1_1 * trkNum)];
//       in1_vec[i] = in1[i];
//     }

//     hls::vector<input_t,N_INPUT_1_1> rot_vec;
    
//     rotationKernel(in1_vec, rot_vec);
//     NN::NNFakeOverlap(rot_vec, out1_vec);

//     for(int i = 0; i < N_LAYER_8; i++){
//       #pragma HLS unroll
//       out1[i] = out1_vec[i];
//     }
//     // for(int i = 0; i < N_LAYER_8; i++){
//     //   #pragma HLS unroll
//     //   out1[i + (trkNum)] = out1_vec[i];
//     // }
//   // }
// }
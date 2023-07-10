#include "globalDefines.h"

// #include "myproject.h"
#include "NNFakeOverlap.h"
#include "rotationKernel.h"

// void full_network(input_raw_t in1[N_INPUT_1_1], result_t out1[N_LAYER_8]);

// void runner(input_raw_t in1_gmem[N_INPUT_1_1 * NUM_TRACKS], result_t out1_gmem[N_LAYER_8 * NUM_TRACKS]){
//   hls::vector<hls::vector<input_raw_t,N_INPUT_1_1>, NUM_TRACKS> in1;
//   hls::vector<hls::vector<layer1_t,N_INPUT_1_1>, NUM_TRACKS> rot;
//   hls::vector<hls::vector<result_t,N_LAYER_8>, NUM_TRACKS> out1;

//   #pragma HLS ARRAY_RESHAPE variable=in1 complete dim = 0
//   #pragma HLS ARRAY_RESHAPE variable=rot complete dim = 0
//   #pragma HLS ARRAY_RESHAPE variable=out1 complete dim = 0

//   for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
//     for(int i = 0; i < N_INPUT_1_1; i++){
//       #pragma HLS unroll 
//       in1[trkNum][i] = in1_gmem[i + (N_INPUT_1_1 * trkNum)];
//     }
//   }

//   for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
//     // #pragma HLS unroll
//     // #pragma HLS PIPELINE 
//     hls::vector<input_raw_t,N_INPUT_1_1> in1_loc;
//     hls::vector<layer1_t,N_INPUT_1_1> rot_loc;
//     in1_loc = in1[trkNum];
//     rotationKernel(in1_loc, rot_loc);
//     rot[trkNum] = rot_loc;
//   }

//   for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
//     // #pragma HLS PIPELINE 
//     hls::vector<layer1_t,N_INPUT_1_1> rot_loc;
//     hls::vector<result_t,N_LAYER_8> out1_loc;
//     rot_loc = rot[trkNum];
//     NN::NNFakeOverlap(rot_loc, out1_loc);
//     out1[trkNum] = out1_loc;
//   }

//   for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
//     for(int i = 0; i < N_LAYER_8; i++){
//       #pragma HLS unroll 
//       out1_gmem[i + (N_LAYER_8 * trkNum)] = out1[trkNum][i];
//     }
//   }

// }


void runner(input_raw_t in1[N_INPUT_1_1], result_t out1[N_LAYER_8]){
// void runner(input_raw_t in1[N_INPUT_1_1 * NUM_TRACKS], result_t out1[N_LAYER_8 * NUM_TRACKS]){

  #pragma HLS ARRAY_RESHAPE variable=in1 complete dim = 0
  #pragma HLS ARRAY_RESHAPE variable=out1 complete dim = 0
  // #pragma HLS PIPELINE

  hls::vector<input_raw_t,N_INPUT_1_1> in1_vec;
  hls::vector<result_t,N_LAYER_8> out1_vec;
  #pragma HLS ARRAY_RESHAPE variable = in1_vec complete dim = 0
  #pragma HLS ARRAY_RESHAPE variable = out1_vec complete dim = 0

  // for(int trkNum = 0; trkNum < NUM_TRACKS; trkNum++){
  //   #pragma HLS unroll

    for(int i = 0; i < N_INPUT_1_1; i++){
      #pragma HLS unroll 
      // in1_vec[i] = in1[i + (N_INPUT_1_1 * trkNum)];
      in1_vec[i] = in1[i];
    }

    hls::vector<layer1_t,N_INPUT_1_1> rot_vec;
    
    rotationKernel(in1_vec, rot_vec);
    NN::NNFakeOverlap(rot_vec, out1_vec);

    for(int i = 0; i < N_LAYER_8; i++){
      #pragma HLS unroll
      out1[i] = out1_vec[i];
    }
    // for(int i = 0; i < N_LAYER_8; i++){
    //   #pragma HLS unroll
    //   out1[i + (trkNum)] = out1_vec[i];
    // }
  // }
}
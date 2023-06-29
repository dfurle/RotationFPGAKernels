//
//    rfnoc-hls-neuralnet: Vivado HLS code for neural-net building blocks
//
//    Copyright (C) 2017 EJ Kreinar
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef MYPROJECT_H_
#define MYPROJECT_H_

// #include "ap_int.h"
// #include "ap_fixed.h"

#include "defines.h"

#include "hls_stream.h"
#include "hls_vector.h"

extern "C"{
  
// Prototype of top level function for C-synthesis
// void myproject(hls::stream<layer1_t>& in1_func, result_t* out1_func);
void full_network(input_raw_t* in1_func, result_t* out1_func);
// void full_network(input_raw_t in1_func[N_INPUT_1_1], result_t out1_func[N_LAYER_8]);
// void myproject(hls::stream<layer1_t[N_INPUT_1_1]>& in1_func, hls::stream<result_t[N_LAYER_8]>& out1_func){


void myproject(hls::stream<hls::vector<layer1_t,N_INPUT_1_1>>& in1_func, hls::stream<hls::vector<result_t,N_LAYER_8>>& out1_func);


// void myproject(hls::stream<layer1_t>& in1_func, hls::stream<result_t>& out1_func);
// void myproject(layer1_t* in1, result_t* out1);

}

#endif

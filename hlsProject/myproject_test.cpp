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
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "firmware/myproject.h"
#include "firmware/rotationKernel.h"
#include "firmware/nnet_utils/nnet_helpers.h"

// hls-fpga-machine-learning insert bram

#define CHECKPOINT 1

namespace nnet
{
    bool trace_enabled = true;
    std::map<std::string, void *> *trace_outputs = NULL;
    size_t trace_type_size = sizeof(double);
}

int main(int argc, char **argv)
{
//     // load input data from text file
//     std::ifstream fin("tb_data/tb_input_features_raw.dat");
//     std::ifstream finRotated("tb_data/tb_input_features_large.dat");
//     // load predictions from text file
//     std::ifstream fpr("tb_data/tb_output_predictions_large.dat");

#ifdef RTL_SIM
    std::string RESULTS_LOG = "tb_data/rtl_cosim_results.log";
#else
    std::string RESULTS_LOG = "tb_data/csim_results.log";
#endif
    std::ofstream fout(RESULTS_LOG);

    std::cout << "INFO: Unable to open input/predictions file, using default input." << std::endl;

    // hls-fpga-machine-learning insert zero
    input_raw_t input_1[N_INPUT_1_1];
    nnet::fill_zero<input_raw_t, N_INPUT_1_1>(input_1);
    result_t layer9_out[N_LAYER_8];

    // hls-fpga-machine-learning insert top-level-function
    myproject(input_1, layer9_out);

    // hls-fpga-machine-learning insert output
    nnet::print_result<result_t, N_LAYER_8>(layer9_out, std::cout, true);

    // hls-fpga-machine-learning insert tb-output
    nnet::print_result<result_t, N_LAYER_8>(layer9_out, fout);

    fout.close();
    std::cout << "INFO: Saved inference results to file: " << RESULTS_LOG << std::endl;

    return 0;
}

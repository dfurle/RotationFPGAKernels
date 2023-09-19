#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "runner/runner.h"

// hls-fpga-machine-learning insert bram

#define CHECKPOINT 5000

namespace nnet {
bool trace_enabled = true;
std::map<std::string, void *> *trace_outputs = NULL;
size_t trace_type_size = sizeof(double);
} // namespace nnet

int main(int argc, char **argv) {
    // load input data from text file
    std::ifstream fin("tb_data/tb_input_features.dat");
    // load predictions from text file
    std::ifstream fpr("tb_data/tb_output_predictions.dat");

#ifdef RTL_SIM
    std::string RESULTS_LOG = "tb_data/rtl_cosim_results.log";
#else
    std::string RESULTS_LOG = "tb_data/csim_results.log";
#endif
    std::ofstream fout(RESULTS_LOG);

    std::string iline;
    std::string pline;
    int e = 0;

    int num_trk = 10;
    

    input_raw_t* in1 = new input_raw_t[N_INPUT_1_1 * num_trk];

    int index = 0;
    std::string sa;
    for(int j = 0; j < num_trk; j++)
    {
      getline(fin, sa);
      std::istringstream iss(sa);
      std::string s;
      while ( getline( iss, s, ' ' ) ) {
        in1[index] = atof(s.c_str());
        index++;
      }
    }

    result_t* out1 = new result_t[N_LAYER_8 * num_trk];

    runner(in, out1, num_trk);


    for(int j = 0; j < num_trk; j++)
    {
      for(int i = 0; i < N_LAYER_8; i++)
      {
        fout << float(out1[i + N_LAYER_8 * j]) << " ";
      }
      fout << std::endl;
    }


    return 0;
}

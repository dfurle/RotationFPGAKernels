#include "helper.h"

#include <fstream>

#include <iostream>

#include <stdlib.h>

#define HOST

#include "runner.h"

// #include "ap_fixed.h"

void printPythonList(int * arr, size_t size, std::string name);

static
const std::string error_message =
  "Error: Result mismatch:\n"
"i = %d CPU result = %d Device result = %d\n";

int setupDevice(std::vector < cl::Device > & devices, cl::Device & device) {
  bool found_device = false;
  // traversing all Platforms To find Xilinx Platform and targeted
  // Device in Xilinx Platform
  std::vector < cl::Platform > platforms;
  cl::Platform::get( & platforms);
  std::cout << "Scanning Platforms" << std::endl;
  std::cout << "number: " << platforms.size() << std::endl;
  for (size_t i = 0;
    (i < platforms.size()) & (found_device == false); i++) {
    cl::Platform platform = platforms[i];
    std::string platformName = platform.getInfo < CL_PLATFORM_NAME > ();
    std::cout << "Found platform: " << std::endl;
    std::cout << platformName << std::endl;
    if (platformName == "Xilinx") {
      devices.clear();
      platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, & devices);
      std::cout << "number Devices: " << devices.size() << std::endl;
      if (devices.size()) {
        for (size_t d = 0; d < devices.size(); d++) {
          std::string deviceName = devices[0].getInfo < CL_DEVICE_NAME > ();
          std::cout << "Device: (" << d << "): " << deviceName << std::endl;
          // if(deviceName.find("u280") != std::string::npos){
          device = devices[0];
          found_device = true;
          // break;
          // } else {
          // devices.erase(devices.begin());
          // }
        }
      }
    }
  }
  // return EXIT_FAILURE;
  if (found_device == false) {
    std::cout << "Error: Unable to find Target Device " <<
      device.getInfo < CL_DEVICE_NAME > () << std::endl;
    return EXIT_FAILURE;
  }
  devices.resize(1);
}

int printTiming(std::string str, std::chrono::_V2::system_clock::time_point & begin) {
  auto end = std::chrono::high_resolution_clock::now();
  int diffus = std::chrono::duration_cast < std::chrono::microseconds > (end - begin).count();
  printf(str.c_str(), diffus);
  begin = end;
  return diffus;
}

// #define DOPRINT
// #define PRINT_INNER_TIMINGS

void runFPGA(cl::CommandQueue & q, cl::Kernel & kernel, cl::Buffer buffer_a, cl::Buffer & buffer_result, NN::input_raw_t * ptr_a, NN::result_t * ptr_result, NN::input_raw_t * in1, NN::result_t * out1, size_t in_size, size_t out_nn_size, int num_trk) {

  #ifdef PRINT_INNER_TIMINGS
  auto begin = std::chrono::high_resolution_clock::now();
  #endif

  memcpy(ptr_a, in1, in_size);

  #ifdef PRINT_INNER_TIMINGS
  printTiming(" |Mem Copy In:\n |  %d us\n", begin);
  #endif

  // std::cout << "setting input data" << std::endl;
  // for(int j = 0; j < num_trk; j++){
  //   for(int i = 0; i < N_INPUT_1_1; i++){
  //     std::cout << ptr_a[i + N_INPUT_1_1 * j] << " ";
  //   }
  //   std::cout << std::endl;
  // }
  // #ifdef PRINT_INNER_TIMINGS
  // printTiming(" |Print In Data:\n |  %d us\n", begin);
  // #endif

  // Data will be migrated to kernel space
  cl::Event write_event;
  q.enqueueMigrateMemObjects({
    buffer_a
  }, 0, NULL, & write_event); // 0 means from host

  // Launch the Kernel
  cl::Event run_event;
  q.enqueueTask(kernel, NULL, & run_event);

  // This call will transfer the data from FPGA to host array
  cl::Event read_event;
  q.enqueueMigrateMemObjects({
    buffer_result
  }, CL_MIGRATE_MEM_OBJECT_HOST, NULL, & write_event);

  #ifdef PRINT_INNER_TIMINGS
  printTiming(" |enqueue:\n |  %d us\n", begin);
  #endif

  q.finish();

  cl_ulong write_start = write_event.getProfilingInfo < CL_PROFILING_COMMAND_START > ();
  cl_ulong write_end = write_event.getProfilingInfo < CL_PROFILING_COMMAND_END > ();
  cl_ulong write_time = write_end - write_start;
  std::cout << "[HOST] Write buffer time is " << std::dec << write_time << " ns" << std::endl;

  cl_ulong run_start = run_event.getProfilingInfo < CL_PROFILING_COMMAND_START > ();
  cl_ulong run_end = run_event.getProfilingInfo < CL_PROFILING_COMMAND_END > ();
  cl_ulong run_time = run_end - run_start;
  std::cout << "[HOST] Kernel run time is " << std::dec << run_time << " ns" << std::endl;

  cl_ulong read_start = read_event.getProfilingInfo < CL_PROFILING_COMMAND_START > ();
  cl_ulong read_end = read_event.getProfilingInfo < CL_PROFILING_COMMAND_END > ();
  cl_ulong read_time = read_end - read_start;
  std::cout << "[HOST] Read buffer time is " << std::dec << read_time << " ns" << std::endl;

  #ifdef PRINT_INNER_TIMINGS
  printTiming(" |Called FPGA:\n |  %d us\n", begin);
  #endif

  memcpy(out1, ptr_result, out_nn_size);

  #ifdef PRINT_INNER_TIMINGS
  printTiming(" |Mem Copy out:\n |  %d us\n", begin);
  #endif

  // std::cout << "Result: \n\n";
  // for(int j = 0; j < num_trk; j++){
  //   for(int i = 0; i < N_LAYER_8; i++){
  //     std::cout << ptr_result[i + N_LAYER_8 * j] << " ";
  //   }
  //   std::cout << std::endl;
  // }
  // #ifdef PRINT_INNER_TIMINGS
  // printTiming(" |Print Output:\n |  %d us\n", begin);
  // #endif
}

int setupRun(cl::Program & program, cl::Context & context, cl::CommandQueue & q, NN::input_raw_t * in1, NN::result_t * out1, int num_trk) {
  auto begin = std::chrono::high_resolution_clock::now();

  size_t in_size = sizeof(NN::input_raw_t) * N_INPUT_1_1 * num_trk;
  cl::Buffer buffer_a(context, CL_MEM_READ_ONLY, in_size);

  printTiming("Buffer In:\n  %d us\n", begin);

  NN::input_raw_t * ptr_a = (NN::input_raw_t * ) q.enqueueMapBuffer(buffer_a, CL_TRUE, CL_MAP_WRITE, 0, in_size);

  printTiming("EnqueueMap In:\n  %d us\n", begin);

  size_t out_nn_size = sizeof(NN::result_t) * N_LAYER_8 * num_trk;
  cl::Buffer buffer_result(context, CL_MEM_WRITE_ONLY, out_nn_size);
  NN::result_t * ptr_result = (NN::result_t * ) q.enqueueMapBuffer(buffer_result, CL_TRUE, CL_MAP_READ, 0, out_nn_size);

  printTiming("EnqueueMap Out:\n  %d us\n", begin);

  cl::Kernel rkernel(program, "runner");

  // set the kernel Arguments
  int narg = 0;
  rkernel.setArg(narg++, buffer_a);
  rkernel.setArg(narg++, buffer_result);
  rkernel.setArg(narg++, num_trk);

  printTiming("Initializing:\n  %d us\n", begin);

  int num_sends = 1;
  printf(" ┌------- sending %dx of %d tracks \n", num_sends, num_trk);
  for (int i = 0; i < num_sends; i++) {
    runFPGA(q, rkernel, buffer_a, buffer_result, ptr_a, ptr_result, in1, out1, in_size, out_nn_size, num_trk);
  }
  std::cout << " └>Total on FPGA+transfer run: " << num_sends << " sends\n      ";
  int timingFPGA = printTiming("%d us\n", begin);
  std::cout << "       per send : " << timingFPGA / num_sends << " us" << std::endl;
  std::cout << "       per track: " << float(1000 * timingFPGA / num_sends) / num_trk << " ns !!" << std::endl;

  q.enqueueUnmapMemObject(buffer_a, ptr_a);
  q.enqueueUnmapMemObject(buffer_result, ptr_result);
  q.finish();

  printTiming("enqueueUnmap:\n  %d us\n", begin);
  return timingFPGA / num_sends;
}

int main(int argc, char * argv[]) 
{

  // TARGET_DEVICE macro needs to be passed from gcc command line
  if (argc < 2 || argc > 3) {
    std::cout << "Usage: " << argv[0] << "<xclbin>" << std::endl;
    return EXIT_FAILURE;
  }
  // Creates a vector of DATA_SIZE elements with an initial value of 10 and 32
  // using customized allocator for getting buffer alignment to 4k boundary
  std::vector < cl::Device > devices;
  cl::Device device;
  if (setupDevice(devices, device) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  // Creating Context and Command Queue for selected device
  cl::Context context(device);
  cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);

  // ======= Found Device =======================================================================================

  // Load rotation xclbin
  char * rxclbinFilename = argv[1];
  std::cout << "Loading: '" << rxclbinFilename << "'\n";
  std::ifstream rbin_file(rxclbinFilename, std::ifstream::binary);
  rbin_file.seekg(0, rbin_file.end);
  unsigned rnb = rbin_file.tellg();
  rbin_file.seekg(0, rbin_file.beg);
  char * rbuf = new char[rnb];
  rbin_file.read(rbuf, rnb);

  // Creating Program from Binary File
  cl::Program::Binaries bins;
  bins.push_back({
    rbuf,
    rnb
  });
  cl::Program program(context, devices, bins);

  // This call will get the kernel object from program. A kernel is an
  // OpenCL function that is executed on the FPGA.

  std::cout<<"---- Size of Stream: "<< INPUTTRACKSIZE<<std::endl;
  std::cout<<"---- Size of Stream word: "<< STREAMWORDSIZE<<std::endl;
  std::cout<<"---- Bit size of one hit: "<< std::numeric_limits<float>::digits<<std::endl;


  printf("INITIALIZING DATA\n");

  int num_trk = INPUTTRACKSIZE;
  printf("\n\n\nRunning with %d tracks\n", num_trk);

  std::ifstream file("../runner/tb_data/tb_input_features_original.dat");
  std::string sa;

  NN::input_raw_t * in1 = new NN::input_raw_t[N_INPUT_1_1 * num_trk];

  int skip_first = 0; // skip first few tracks

  if (argc > 2) {
    skip_first = std::stod(argv[2]);
    std::cout << "read argv" << std::endl;
  }

  std::cout << "skip first: #" << skip_first << std::endl;
  for (int d = 0; d < skip_first; d++) {
    getline(file, sa);
  }

  auto begin = std::chrono::high_resolution_clock::now();

  int index = 0;
  for (int j = 0; j < num_trk; j++) {
    getline(file, sa);
    std::istringstream iss(sa);
    std::string s;
    while (getline(iss, s, ' ')) {
      in1[index] = atof(s.c_str());
      index++;
    }
  }

   for (int j = 0; j < INPUTTRACKSIZE; j++) 
    {  
      std::cout<<"Track : "<<j<<" ";
      for(int i = 0; i < N_INPUT_1_1; i++)
      {
        std::cout<<in1[i + (N_INPUT_1_1 * j)]<<" ";
      }
      std::cout<<std::endl;
    }

  printTiming("Read File and Convert\n  %d us \n", begin);

  NN::result_t * out1 = new NN::result_t[N_LAYER_8 * num_trk];
  printf("\nRUNNING FPGA\n\n");

  printTiming("Init out1\n  %d us \n", begin);
  int timingFPGA = setupRun(program, context, q, in1, out1, num_trk);
  auto track_timing_full = timingFPGA;
  auto track_timing_single = (1000. * timingFPGA) / num_trk;
  printTiming("Finished %d us\n", begin);

  std::cout << "Result: \n\n";
  for (int j = 0; j < num_trk; j++) {
    for (int i = 0; i < N_LAYER_8; i++) {
      std::cout << "j: "<<j<<" "<<float(out1[i + N_LAYER_8 * j]) << " ";
    }
    std::cout << std::endl;
  }
  printTiming("Printing %d us\n", begin);

  printf("\n\n");

  printf("num tracks: %d\n", num_trk);
  printf("  full: %d us\n", track_timing_full);
  printf("  trk : %d ns\n", track_timing_single);

  printf("\n");

  return 0;
}
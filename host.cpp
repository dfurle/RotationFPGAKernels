/*******************************************************************************
Vendor: Xilinx
Associated Filename: vadd.cpp
Purpose: VITIS vector addition

*******************************************************************************
Copyright (C) 2019 XILINX, Inc.

This file contains confidential and proprietary information of Xilinx, Inc. and
is protected under U.S. and international copyright and other intellectual
property laws.

DISCLAIMER
This disclaimer is not a license and does not grant any rights to the materials
distributed herewith. Except as otherwise provided in a valid license issued to
you by Xilinx, and to the maximum extent permitted by applicable law:
(1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND XILINX
HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY,
INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR
FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether
in contract or tort, including negligence, or under any other theory of
liability) for any loss or damage of any kind or nature related to, arising
under or in connection with these materials, including for any direct, or any
indirect, special, incidental, or consequential loss or damage (including loss
of data, profits, goodwill, or any type of loss or damage suffered as a result
of any action brought by a third party) even if such damage or loss was
reasonably foreseeable or Xilinx had been advised of the possibility of the
same.

CRITICAL APPLICATIONS
Xilinx products are not designed or intended to be fail-safe, or for use in any
application requiring fail-safe performance, such as life-support or safety
devices or systems, Class III medical devices, nuclear facilities, applications
related to the deployment of airbags, or any other applications that could lead
to death, personal injury, or severe property or environmental damage
(individually and collectively, "Critical Applications"). Customer assumes the
sole risk and liability of any use of Xilinx products in Critical Applications,
subject only to applicable laws and regulations governing limitations on product
liability.

THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT
ALL TIMES.

*******************************************************************************/
#include "helper.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "ap_fixed.h"


static const int DATA_SIZE = 1024; // 4096;

#define N_INPUT_1_1 24
#define N_LAYER_8 1


static const std::string error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";

int setupDevice(std::vector<cl::Device>& devices, cl::Device& device){
  bool found_device = false;
  // traversing all Platforms To find Xilinx Platform and targeted
  // Device in Xilinx Platform
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  std::cout << "Scanning Platforms" << std::endl;
  std::cout << "number: " << platforms.size() << std::endl;
  for (size_t i = 0; (i < platforms.size()) & (found_device == false); i++) {
    cl::Platform platform = platforms[i];
    std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
    std::cout << platformName << std::endl;
    if (platformName == "Xilinx") {
      devices.clear();
      platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
      if (devices.size()) {
        device = devices[0];
        found_device = true;
        break;
      }
    }
  }
  if (found_device == false) {
    std::cout << "Error: Unable to find Target Device "
              << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    return EXIT_FAILURE;
  }
  devices.resize(1);
}

void setupRunRotation(cl::Program& program, cl::Context& context, cl::CommandQueue& q, float *in1, float *out1){

  cl::Kernel rkernel(program, "rotationKernel");

  // Compute the size of array in bytes
  size_t in_size = sizeof(ap_fixed<16,11>) * N_INPUT_1_1;
  size_t out_size = sizeof(ap_fixed<16,6>) * N_INPUT_1_1;

  std::cout << "in_size:  " << in_size << std::endl;
  std::cout << "out_size: " << out_size << std::endl;

  // These commands will allocate memory on the Device. The cl::Buffer objects
  // can be used to reference the memory locations on the device.
  cl::Buffer buffer_a(context, CL_MEM_READ_ONLY, in_size);
  cl::Buffer buffer_result(context, CL_MEM_WRITE_ONLY, out_size);

  // set the kernel Arguments
  int narg = 0;
  rkernel.setArg(narg++, buffer_a);
  rkernel.setArg(narg++, buffer_result);

  // We then need to map our OpenCL buffers to get the pointers
  ap_fixed<16,11> *ptr_a = (ap_fixed<16,11> *)q.enqueueMapBuffer(buffer_a, CL_TRUE, CL_MAP_WRITE, 0,
                                         in_size);
  ap_fixed<16,6> *ptr_result = (ap_fixed<16,6> *)q.enqueueMapBuffer(buffer_result, CL_TRUE,
                                              CL_MAP_READ, 0, out_size);

  std::cout << "setting input data" << std::endl;

  // setting input data
  for (int i = 0; i < N_INPUT_1_1; i++) {
    ptr_a[i] = in1[i];
    std::cout << ptr_a[i] << " ";
  }
  std::cout << std::endl;

  // Data will be migrated to kernel space
  q.enqueueMigrateMemObjects({buffer_a}, 0 /* 0 means from host*/);

  // Launch the Kernel
  q.enqueueTask(rkernel);

  // The result of the previous kernel execution will need to be retrieved in
  // order to view the results. This call will transfer the data from FPGA to
  // source_results vector
  q.enqueueMigrateMemObjects({buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST);

  q.finish();

  // setting input data
  for (int i = 0; i < N_INPUT_1_1; i++) {
    out1[i] = ptr_result[i];
    std::cout << out1[i] << " ";
  }
  std::cout << std::endl;

  q.enqueueUnmapMemObject(buffer_a, ptr_a);
  q.enqueueUnmapMemObject(buffer_result, ptr_result);
  q.finish();
}

void setupRunNN(cl::Program& program, cl::Context& context, cl::CommandQueue& q, float *in1, float *out1){
  cl::Kernel nkernel(program, "myproject");

  // Compute the size of array in bytes
  size_t in_size = sizeof(ap_fixed<16,6>) * N_INPUT_1_1;
  size_t out_size = sizeof(ap_fixed<16,6>) * N_LAYER_8;

  std::cout << "in_size:  " << in_size << std::endl;
  std::cout << "out_size: " << out_size << std::endl;

  // These commands will allocate memory on the Device. The cl::Buffer objects
  // can be used to reference the memory locations on the device.
  cl::Buffer buffer_a(context, CL_MEM_READ_ONLY, in_size);
  cl::Buffer buffer_result(context, CL_MEM_WRITE_ONLY, out_size);

  // set the kernel Arguments
  int narg = 0;
  nkernel.setArg(narg++, buffer_a);
  nkernel.setArg(narg++, buffer_result);

  // We then need to map our OpenCL buffers to get the pointers
  ap_fixed<16,6> *ptr_a = (ap_fixed<16,6> *)q.enqueueMapBuffer(buffer_a, CL_TRUE, CL_MAP_WRITE, 0,
                                         in_size);
      

  ap_fixed<16,6> *ptr_result = (ap_fixed<16,6> *)q.enqueueMapBuffer(buffer_result, CL_TRUE,
                                              CL_MAP_READ, 0, out_size);


  std::cout << "setting input data" << std::endl;

  // setting input data
  for (int i = 0; i < N_INPUT_1_1; i++) {
    ptr_a[i] = in1[i];
    std::cout << ptr_a[i] << " ";
  }
  std::cout << std::endl;

  // Data will be migrated to kernel space
  q.enqueueMigrateMemObjects({buffer_a}, 0 /* 0 means from host*/);

  // Launch the Kernel
  q.enqueueTask(nkernel);

  // The result of the previous kernel execution will need to be retrieved in
  // order to view the results. This call will transfer the data from FPGA to
  // source_results vector
  q.enqueueMigrateMemObjects({buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST);

  q.finish();


  // setting input data
  for (int i = 0; i < N_LAYER_8; i++) {
    out1[i] = ptr_result[i];
    std::cout << out1[i] << " ";
  }
  std::cout << std::endl;

  q.enqueueUnmapMemObject(buffer_a, ptr_a);
  q.enqueueUnmapMemObject(buffer_result, ptr_result);
  q.finish();
}

int main(int argc, char *argv[]) {

  // TARGET_DEVICE macro needs to be passed from gcc command line
  if (argc < 2 || argc > 3) {
    std::cout << "Usage: " << argv[0] << "<xclbin>" << std::endl;
    return EXIT_FAILURE;
  }
  // Creates a vector of DATA_SIZE elements with an initial value of 10 and 32
  // using customized allocator for getting buffer alignment to 4k boundary
  std::vector<cl::Device> devices;
  cl::Device device;
  if(setupDevice(devices, device) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }

  // Creating Context and Command Queue for selected device
  cl::Context context(device);
  cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);

  // ======= Found Device =======================================================================================

  // Load rotation xclbin
  char *rxclbinFilename = argv[1];
  std::cout << "Loading: '" << rxclbinFilename << "'\n";
  std::ifstream rbin_file(rxclbinFilename, std::ifstream::binary);
  rbin_file.seekg(0, rbin_file.end);
  unsigned rnb = rbin_file.tellg();
  rbin_file.seekg(0, rbin_file.beg);
  char *rbuf = new char[rnb];
  rbin_file.read(rbuf, rnb);


  // Creating Program from Binary File
  cl::Program::Binaries bins;
  bins.push_back({rbuf, rnb});
  cl::Program program(context, devices, bins);

  // This call will get the kernel object from program. A kernel is an
  // OpenCL function that is executed on the FPGA.

  printf("INITIALIZING DATA\n");

  // float* in1 = new float[N_INPUT_1_1];
  // for(int i = 0; i < N_INPUT_1_1; i++){
  //   in1[i] = i * 2.346547; // some random decimal to produce some decimal results
  // }
  
  std::ifstream file("inputs.dat");
  std::string sa;
  float* in1 = new float[N_INPUT_1_1];

  int dataset = 1; // which dataset to read from file, terribly efficiency wise; there are 3 (1,2,3)

  if(argc > 2){
    dataset = std::stod(argv[2]);
    std::cout << "read argv" << std::endl;
  }

  std::cout << "dataset: #" << dataset << std::endl;

  for(int d = 0; d < dataset; d++){
    for(int i = 0; i < N_INPUT_1_1; i++){
      getline(file, sa);
      in1[i] = std::stof(sa);
      // in1[i] = i * 2.346547; // some random decimal to produce some decimal results
    }
  }

  float* out1 = new float[N_INPUT_1_1];
  printf("\nRUNNING ROTATION\n\n");
  setupRunRotation(program, context, q, in1, out1);

  float* in2 = out1;
  float* out2 = new float[N_INPUT_1_1];
  printf("\nRUNNING NN\n\n");
  setupRunNN(program, context, q, in2, out2);

  float* out = out2;

  std::cout << "Finished" << std::endl;

  // Verify the result
  int match = 0;
  for (int i = 0; i < N_LAYER_8; i++) {
    std::cout << out[i] << std::endl;
  }
  std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
  return 0;
}

//<copyright-disclaimer-start>
//  ************************************************************************************************************
//  * © Copyright 2022 Xilinx, Inc. All rights reserved. *
//  * This file contains confidential and proprietary information of Xilinx,
//  Inc. and                          *
//  * is protected under U.S. and international copyright and other intellectual
//  property laws.                *
//  * DISCLAIMER *
//  * This disclaimer is not a license and does not grant any rights to the
//  materials distributed              *
//  * herewith. Except as otherwise provided in a valid license issued to you by
//  Xilinx, and to the            *
//  * maximum extent permitted by applicable law: (1) THESE MATERIALS ARE MADE
//  AVAILABLE "AS IS"               *
//  * AND WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND
//  CONDITIONS, EXPRESS,                 *
//  * IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
//  MERCHANTABILITY, NON-INFRINGEMENT,     *
//  * OR FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable
//  (whether in contract or tort,  *
//  * including negligence, or under any other theory of liability) for any loss
//  or damage of any kind or      *
//  * nature related to, arising under or in connection with these materials,
//  including for any direct, or     *
//  * any indirect, special, incidental, or consequential loss or damage
//  (including loss of data, profits,     *
//  * goodwill, or any type of loss or damage suffered as a result of any action
//  brought by a third party)     *
//  * even if such damage or loss was reasonably foreseeable or Xilinx had been
//  advised of the possibility     *
//  * of the same. *
//  * CRITICAL APPLICATIONS *
//  * Xilinx products are not designed or intended to be fail-safe, or for use
//  in any application requiring    *
//  * fail-safe performance, such as life-support or safety devices or systems,
//  Class III medical devices,     *
//  * nuclear facilities, applications related to the deployment of airbags, or
//  any other applications that    *
//  * could lead to death, personal injury, or severe property or environmental
//  damage (individually and       *
//  * collectively, "Critical Applications"). Customer assumes the sole risk and
//  liability of any use of       *
//  * Xilinx products in Critical Applications, subject only to applicable laws
//  and regulations governing      *
//  * limitations on product liability. *
//  * THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
//  AT ALL TIMES.                 *
//  * *
//  ************************************************************************************************************
//<copyright-disclaimer-end>

# Rotation Kernel

## What this does

Project contains a neural network compiled by `hls4ml` from keras python (`./hlsProject/`). Initially that project would contain just the NN implementation with fixed size array arguments, but was further edited to allow different parameters for testing.

Project also contains the `./rotationKernel/` which does some preprocessing on the data to be passed into the NN.

Further, project contains a "runner" function (`./runner/runner.cpp`). Which calls on the `rotationKernel` and `hlsProject` in sequence.

Initial idea was to seperate everything into their own "kernels" such that each is generated as a `.xo` (xilinx's object format) which then could be linked together into the `.xclbin` to be uploaded to FPGAs. This `.xclbin` will be uploaded by the CPU (using OpenCL library) to a PCIe connected FPGA card (Data Center Cards, ex: u200) and data will be sent by CPU through PCIe to FPGA to execute.

We have not found much information on how to properly link together the kernels to execute sequentially. The closest we got was enqueueing multiple kernels after each other which we don't know the performance of compared to compiling a single kernel yet.

Thus the final setup is having two functions act as virtual kernels to the "top" function which will be called by the Host, which then calls the virtual kernels sequentially, and thus the data passing is definitely optimized for FPGA. Furthermore it is easier to see the total time of execution this way.

## Setup

### Pre-requisites

- Vitis 2023.1 or Vitis 2022.2
- (probably Ubuntu 22.04)
- XRT library 2023.1
- Depending on what platform deployment is needed for:
  - Xilinx u200 board Deployment Target Platform
  - Xilinx u200 board Development Target Platform
  - see [this link](https://www.xilinx.com/products/boards-and-kits/alveo/u200.html#gettingStarted) to download the last three files

### First time setup

- copy the template build directory to start building
- `cp -r template_build build`
- `cd build`

### On every terminal

- `source {Vitis}/2023.1/settings64.sh`
  - or `source {Vitis}/2022.2/settings64.sh`
- `source /opt/xilinx/xrt/setup.sh`

### Every change

- run `make` (or do step by step)
  - `make host`
  - `make compile`
  - `make link`
    - `make clean` deletes all compiled files
    - `make all` cleans all main files
- run `source export.sh` to get XCL_EMULATION_MODE=(mode) (if `EMU_MODE` was changed)
- run `./host.exe TrackClassification.xclbin`

### Checking performance

- you can switch the `debug_mode` from `off` to `gui` in `xrt.ini` to show some extra run information
- otherwise, you can run `vitis_analyzer` and open the main compilation and profiling file
  - `./{build}/kernels/runner/runner.hlscompile_summary`
  - the `Schedule Viewer` seem to be a fairly important tab to see how the virtual kernels operate

## Settings to change when adjusting

- in `./runner/globalDefines.h` can edit various parameter modes (choose only 1)
  - `#define STREAM` uses `hls::stream<hls::vector>` between every function call, and converts input array to vector
  - `#define MID_STREAM` uses `hls::stream<hls::vector>` to stream from `rotationKernel` to `hlsProject`, and converts input array to vector
  - `#define VECTOR` uses `hls::vector` between every function call, and converts input array to vector
  - `#define ARRAY` uses simple fixed arrays, and loads input array into local array

- in the Makefile you can edit various things to add/change
  - `HLS_NAME` changes name of kernel and output files
    - if changing, also have to change `config.cfg` file
  - `PART` should be set to the platform that you installed (from .deb/.rpm)
    - (currently only works for Data Center cards, not Embedded (requires --sysroot etc))
    - when changing also change `part` option in `./runner/hls_config.cfg`
      - still don't know where to find `part=` id name, found current one through Vitis IDE GUI when compiling a project...
  - `EMU_MODE` whenever you want to change emulation/hw mode change this between
    - `sw_emu`, `hw_emu`, and `hw`
  - .
  - `HOST_SRC` to add/remove source files for host (.cpp files)
  - `NN_SRC` to add/remove source files for NN kernel (.cpp files)
    - also edit `./runner/hls_config.cfg`
  - `ROTATION_SRC` to add/remove source files for rotation kernel (.cpp files)
    - also edit `./runner/hls_config.cfg`
- when editing one file, make sure the other has similar names
  - such that `hls_config.cfg`, `config.cfg`, and `Makefile` contain same names for arguments that are needed to be same, otherwise might fail to build!
- ! ! ! Important, when running `sw_emu`:
  - inside `hlsProject/firmware/parameters.h`
  - uncomment the `#define __SYNTHESIS__` and `#undef __SYNTHESIS__` around the weight includes
  - reason: during `sw_emu` it doesn't set the `__SYNTHESIS__` define, thus the weights are never initialized and will be `0`. There used to be code to read from text files, but it is simpler to just leave it as is without the extra text files.
  - reason #2: I decided I would rather add the #defines around the includes than go into each weight.h file manually to remove the #ifdefs, since this project could be made to regenerate later on
  - if this is tedious, just go into each weight.h file under `hlsProject/firmware/weights/*.h` and remove the #ifdefs and the weight declaration, and just leave the one with definition initialization,

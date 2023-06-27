# Rotation Kernel

## Setup

### Pre-requisites

- Vitis 2023.1 or Vitis 2022.2
- (probably Ubuntu 22.04)
- XRT library 2023.1
- Xilinx u200 board Deployment Target Platform
- Xilinx u200 board Development Target Platform
- see [this link](https://www.xilinx.com/products/boards-and-kits/alveo/u200.html#gettingStarted) to download the last three files

### First time setup

- `mkdir build`
- `cp Makefile build/`
- `cp hlsProject/tb_data/tb_input_features_raw.dat build/inputs.dat`

### On every terminal

- `source {Vitis}/2023.1/settings64.sh` or `source {Vitis}/2022.2/settings64.sh`
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

## Settings to change when adjusting

- in the Makefile you can edit various things to add/change
  - `HLS_NAME` changes name of kernel and output files
    - if changing, also have to change `config.cfg` file
  - `PART` should be set to the platform that you installed (from .deb/.rpm)
    - (currently only works for Data Center cards, not Embedded (requires --sysroot etc))
  - most important `EMU_MODE` whenever you want to change emulation/hw mode change this between
    - `sw_emu`, `hw_emu`, and `hw`
  - .
  - `HOST_SRC` to add/remove source files for host (.cpp files)
  - `NN_SRC` to add/remove source files for NN kernel (.cpp files)
  - `ROTATION_SRC` to add/remove source files for rotation kernel (.cpp files)

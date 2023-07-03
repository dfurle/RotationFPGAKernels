
# name of .xcl file
HLS_NAME := TrackClassification
# xilinx part to be used
PART := xilinx_u200_gen3x16_xdma_2_202110_1
# emulation mode (sw_emu, hw_emu, hw)
EMU_MODE := hw_emu

########

WORK := $(realpath ../)
LDFLAGS := 
# LDFLAGS += -lxrt_coreutil  # used in xrt instead of opencl implementation, not needed
LDFLAGS += -lxilinxopencl
# LDFLAGS += -pthread   #doesnt seem to be needed, but if anything breaks try uncomment this first!

main: ${HLS_NAME}.xclbin ./host.exe export.sh

.PHONY: host compile link clean all

all: host compile link

HOST_SRC := ${WORK}/host.cpp
host.exe: $(HOST_SRC)
	g++ -g -std=c++17 -Wall -O0 $(HOST_SRC) -o ./host.exe -I${XILINX_XRT}/include/ -I${XILINX_VITIS}/../../Vitis_HLS/2023.1/include/ -L${XILINX_XRT}/lib ${LDFLAGS}
	
NN_DIR := ${WORK}/hlsProject
NN_SRC := ${NN_DIR}/firmware/myproject.cpp
NN_INCLUDE := -I${NN_DIR} -I${NN_DIR}/firmware
	
ROTATION_DIR := ${WORK}/rotationKernel
ROTATION_SRC := ${ROTATION_DIR}/rotationKernel.cpp
ROTATION_INCLUDE := -I${ROTATION_DIR}

# KERNEL_SRC := ${NN_SRC}
# KERNEL_INCLUDE := ${NN_INCLUDE}
# KERNEL_SRC := ${NN_SRC}
# KERNEL_INCLUDE := ${NN_INCLUDE}
KERNEL_SRC := ${NN_SRC} ${ROTATION_SRC}
KERNEL_INCLUDE := ${NN_INCLUDE} ${ROTATION_INCLUDE}
KERNEL_CFG := ${WORK}/hls_config.cfg
KERNEL_NAME := full_network

${KERNEL_NAME}.xo: ${KERNEL_SRC} ${KERNEL_CFG}
	mkdir -p ./kernels
	cd ./kernels; \
	v++ -c --mode hls --config ${KERNEL_CFG}; \
	mv ./${KERNEL_NAME}/${KERNEL_NAME}.xo ..
# broke summary reading when tried to move, just load
# `vitis_analyzer kernels/${KERNEL_NAME}/${KENRLE_NAME}.hlscompile_summary` should have everything needed
# TODO: could make auto generated script
# mv ./kernels/${KERNEL_NAME}/${KERNEL_NAME}.hlscompile_summary ./_${KERNEL_NAME}.hlscompile_summary
#	mv ./kernels/${KERNEL_NAME}/reports/hls_compile.rpt ./_${KERNEL_NAME}_design.rpt

${HLS_NAME}.xclbin: $(KERNEL_NAME).xo emconfig.json ${WORK}/config.cfg
	mkdir -p ./xclbin
	cd ./xclbin; \
	v++ -l -t $(EMU_MODE) --platform $(PART) --config ${WORK}/config.cfg ../$(KERNEL_NAME).xo -o $(HLS_NAME).xclbin; \
	mv ${HLS_NAME}.xclbin ..

export.sh: ${HLS_NAME}.xclbin
	@echo "export XCL_EMULATION_MODE=$(EMU_MODE)\n" > export.sh
	echo "do 'source export.sh' if EMU_MODE was changed"

emconfig.json:
	emconfigutil --platform $(PART)

host: host.exe
	rm -f host.exe
	${MAKE}

link: ${HLS_NAME}.xclbin
	rm -f ${HLS_NAME}.xclbin
	${MAKE}

compile: ${KERNEL_NAME}.xo
	rm -f ${KERNEL_NAME}.xo
	${MAKE}

clean:
	rm -rf ./.Xil/
	rm -rf ./.run/
	rm -rf ./kernels
	rm -rf ./xclbin
	rm -f *.xo
	rm -f *.xclbin
	rm -f *.exe
	rm -f emconfig.json
	rm -f export.sh
	rm -f host.exe
	rm -f xilinx_u200_gen*
	rm -f xrt.run_summary
	rm -f emulation_debug.log
	rm -f *.csv
	rm -rf logs


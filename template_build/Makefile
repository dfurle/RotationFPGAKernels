
# name of .xcl file
HLS_NAME := TrackClassification
# xilinx part to be used
PART := xilinx_u250_gen3x16_xdma_4_1_202210_1
# emulation mode (sw_emu, hw_emu, hw)
EMU_MODE := sw_emu

########

WORK := $(realpath ../)
BUILDPATH := $(realpath ./)
LDFLAGS := 
# LDFLAGS += -lxrt_coreutil  # used in xrt instead of opencl implementation, not needed
LDFLAGS += -lxilinxopencl
# LDFLAGS += -pthread   #doesnt seem to be needed, but if anything breaks try uncomment this first!



main: ${HLS_NAME}.xclbin ./host.exe export.sh

.PHONY: host compile link clean all

all: host compile link


NN_DIR := ${WORK}/hlsProject
NN_SRC := ${NN_DIR}/firmware/NNFakeOverlap.cpp
NN_INCLUDE := -I${NN_DIR} -I${NN_DIR}/firmware
	
ROTATION_DIR := ${WORK}/rotationKernel
ROTATION_SRC := ${ROTATION_DIR}/rotationKernel.cpp
ROTATION_INCLUDE := -I${ROTATION_DIR}

KERNEL_SRC := ${WORK}/runner/runner.cpp ${NN_SRC} ${ROTATION_SRC}
KERNEL_INCLUDE := -I${WORK}/runner ${NN_INCLUDE} ${ROTATION_INCLUDE}
KERNEL_CFG := ${WORK}/runner/hls_config.cfg
KERNEL_NAME := runner

HOST_SRC := ${WORK}/host.cpp
host.exe: $(HOST_SRC)
	g++ -g -std=c++11 -Wall -O0 $(HOST_SRC) -o ./host.exe ${KERNEL_INCLUDE} -I${XILINX_XRT}/include/ -I${XILINX_HLS}/include/ -L${XILINX_XRT}/lib ${LDFLAGS} -DADDWEIGHT
	

${KERNEL_NAME}.xo: ${KERNEL_SRC} ${KERNEL_CFG}
	mkdir -p ./kernels
#v++ -c --mode hls --config ${KERNEL_CFG} &&
	cd ./kernels; \
	v++ -c -t ${EMU_MODE} --platform ${PART} -k ${KERNEL_NAME} ${KERNEL_INCLUDE} ${KERNEL_SRC} -o ${KERNEL_NAME}.xo -DADDWEIGHT && \
	mv ${BUILDPATH}/kernels/${KERNEL_NAME}.xo ${BUILDPATH}/

${HLS_NAME}.xclbin: $(KERNEL_NAME).xo emconfig.json ${WORK}/runner/config.cfg
	mkdir -p ${BUILDPATH}/xclbin
	cd ${BUILDPATH}/xclbin; \
	v++ -l -t $(EMU_MODE) --platform $(PART) --config ${WORK}/runner/config.cfg ${BUILDPATH}/$(KERNEL_NAME).xo -o $(HLS_NAME).xclbin -DADDWEIGHT && \
	mv ${BUILDPATH}/xclbin/${HLS_NAME}.xclbin ${BUILDPATH}/

export.sh: ${HLS_NAME}.xclbin
	@echo "export XCL_EMULATION_MODE=$(EMU_MODE)\n" > export.sh
	echo "do 'source export.sh' if EMU_MODE was changed"

emconfig.json:
	emconfigutil --platform $(PART)

host:
	rm -f host.exe
	${MAKE}

link:
	rm -f ${HLS_NAME}.xclbin
	${MAKE}

compile:
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


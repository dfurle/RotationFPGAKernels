
# name of .xcl file
HLS_NAME := TrackClassification
# xilinx part to be used
PART := xilinx_u200_gen3x16_xdma_2_202110_1
# emulation mode (sw_emu, hw_emu, hw)
EMU_MODE := sw_emu

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
	g++ -g -std=c++17 -Wall -O0 $(HOST_SRC) -o ./host.exe -I${XILINX_XRT}/include/ -I${XILINX_HLS}/include/ -L${XILINX_XRT}/lib ${LDFLAGS}
	
NN_DIR := ${WORK}/hlsProject
NN_SRC := ${NN_DIR}/firmware/myproject.cpp
NN_INCLUDE := -I${NN_DIR} -I${NN_DIR}/firmware
NN_KERNEL := myproject
$(NN_KERNEL).xo: ${ROTATION_SRC}
	mkdir -p ./kernels
	cd ./kernels; \
	v++ -c -t $(EMU_MODE) --platform $(PART) --config ${NN_DIR}/config.cfg -k $(NN_KERNEL) $(NN_INCLUDE) ${NN_SRC} -o $(NN_KERNEL).xo; \
	mv ${NN_KERNEL}.xo ..
	
ROTATION_DIR := ${WORK}/rotationKernel
ROTATION_SRC := ${ROTATION_DIR}/rotationKernel.cpp
ROTATION_INCLUDE := -I${ROTATION_DIR}
ROTATION_KERNEL := rotationKernel
${ROTATION_KERNEL}.xo: ${ROTATION_SRC}
	mkdir -p ./kernels
	cd ./kernels; \
	v++ -c -t $(EMU_MODE) --platform $(PART) --config ${ROTATION_DIR}/config.cfg -k ${ROTATION_KERNEL} $(ROTATION_INCLUDE) ${ROTATION_SRC} -o ${ROTATION_KERNEL}.xo; \
	mv ${ROTATION_KERNEL}.xo ..

${HLS_NAME}.xclbin: $(NN_KERNEL).xo ${ROTATION_KERNEL}.xo emconfig.json
	mkdir -p ./xclbin
	cd ./xclbin; \
	v++ -l -t $(EMU_MODE) --platform $(PART) --config ${WORK}/config.cfg ../$(NN_KERNEL).xo ../${ROTATION_KERNEL}.xo -o $(HLS_NAME).xclbin; \
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

compile: $(NN_KERNEL).xo rotationKernel.xo
	rm -f $(NN_KERNEL).xo rotationKernel.xo
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


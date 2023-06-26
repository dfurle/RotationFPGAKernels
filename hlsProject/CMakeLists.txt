# add_library(hlsProject firmware/myproject.cpp firmware/rotationKernel.cpp)

# target_include_directories(hlsProject PUBLIC ${XILINX_VITIS}/../../Vitis_HLS/2023.1/include/)

# target_include_directories(hlsProject PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_include_directories(hlsProject PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/firmware)
cmake_minimum_required(VERSION 3.22)
project(hlsProject)


# set(PART xilinx_u200_gen3x16_xdma_2_202110_1)
# # set(HLS_SRC ${CMAKE_SOURCE_DIR}/firmware/*.cpp)
# set(HLS_CONFIG config.cfg)
# set(HLS_NAME myproject)
# set(INCLUDE_PATHS "-Ifirmware")
# set(XILINX_VITIS /home/denis/UBUNTU_TEST/Xilinx/Vitis/2023.1)
# set(HLS_INCLUDE ${XILINX_VITIS}/../../Vitis_HLS/2023.1/include/)

set(EMU_MODE sw_emu)
set(HLS_CONFIG ${CMAKE_SOURCE_DIR}/hlsProject/config.cfg)
set(HLS_NAME myproject)
set(VPP /home/denis/UBUNTU_TEST/Xilinx/Vitis/2023.1/bin/v++)
set(HLS_SRC 
  ${CMAKE_SOURCE_DIR}/hlsProject/firmware/myproject.cpp
  ${CMAKE_SOURCE_DIR}/hlsProject/firmware/rotationKernel.cpp
  )
set(INCLUDE_PATHS "-I${CMAKE_SOURCE_DIR}/hlsProject/firmware")

if("${XILINX_VITIS}" STREQUAL "")
  message("XILINX_VITIS UNDEFINED")
endif()

message(${HLS_SRC})


add_custom_command(
  OUTPUT ${HLS_NAME}.xo
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS ${HLS_SRC}
  # COMMAND echo "HELLO WORLD 1" > ${HLS_NAME}.xo
  # COMMAND ${VPP} -c -t ${EMU_MODE} --platform ${PART} --config ${HLS_CONFIG} -k ${HLS_NAME} ${INCLUDE_PATHS} ${HLS_SRC} -o ../${HLS_NAME}.xo 
  # COMMAND emconfigutil --platform ${PART}
  # COMMAND ${VPP} -c -t ${EMU_MODE} --platform ${PART} --config ${HLS_CONFIG} -k ${HLS_NAME} ${INCLUDE_PATHS} ${CMAKE_SOURCE_DIR}/hlsProject/firmware/myproject.cpp ${CMAKE_SOURCE_DIR}/hlsProject/firmware/rotationKernel.cpp -o ${HLS_NAME}.xo 
  COMMAND ${VPP} -c -t ${EMU_MODE} --platform ${PART} --config ${HLS_CONFIG} -k ${HLS_NAME} ${INCLUDE_PATHS} ${HLS_SRC} -o ${HLS_NAME}.xo 
  VERBATIM
)

add_custom_command(
  OUTPUT emconfig.json
  COMMAND /home/denis/UBUNTU_TEST/Xilinx/Vitis/2023.1/bin/emconfigutil --platform ${PART}
  VERBATIM
)

add_custom_command(
  OUTPUT export.sh
  COMMAND echo export XCL_EMULATION_MODE=$(EMU_MODE) > export.sh
	COMMAND echo "do 'source export.sh' if EMU_MODE was changed"
)

add_custom_command(
  OUTPUT ${HLS_NAME}.xclbin
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS ${HLS_NAME}.xo
          emconfig.json
          export.sh
  # COMMAND echo "HELLO WORLD 2" > ${HLS_NAME}.xclbin
  COMMAND ${VPP} -l -t ${EMU_MODE} --platform ${PART} --config ${HLS_CONFIG} ${HLS_NAME}.xo -o ${HLS_NAME}.xclbin
  VERBATIM
)


add_custom_target(hlsProject DEPENDS ${HLS_NAME}.xclbin)

# target_include_directories(hlsProject PUBLIC ${HLS_INCLUDE})

# set_property(GLOBAL APPEND PROPERTY OBJECT_DEPENDS ${HLS_NAME}.xo)

# add_executable(hlsProject myproject_test.cpp ${HLS_SRC})
# message(${HLS_INCLUDE})

cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PART xilinx_u200_gen3x16_xdma_2_202110_1)
# set(HLS_SRC ${CMAKE_SOURCE_DIR}/firmware/*.cpp)
set(XILINX_VITIS /home/denis/UBUNTU_TEST/Xilinx/Vitis/2023.1)
set(HLS_INCLUDE ${XILINX_VITIS}/../../Vitis_HLS/2023.1/include/)
set(EMU_MODE sw_emu)

set(XILINX_XRT /opt/xilinx/xrt)




project(Host VERSION 1.0)
add_subdirectory(hlsProject)

add_executable(Host host.cpp)

add_dependencies(Host hlsProject)

target_include_directories(Host PUBLIC ${XILINX_VITIS}/../../Vitis_HLS/2023.1/include/)
target_include_directories(Host PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_directories(Host PUBLIC ${XILINX_XRT}/lib)
target_link_libraries(Host PUBLIC xilinxopencl pthread)


# add_executable(FPGAProject)

# target_link_libraries(FPGAProject PUBLIC Host)

# target_include_directories(FPGAProject PUBLIC
#   "${PROJECT_BINARY_DIR}"
#   "${PROJECT_SOURCE_DIR}/Host"
# )

# add_library(Host host.cpp)


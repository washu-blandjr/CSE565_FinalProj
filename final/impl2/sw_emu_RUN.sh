#!/bin/bash

############################## Test Configuration ##############################
# Choose which test to run by setting these variables
TEST_DIR="benchmark_chiimg"        # Options: benchmark_chiimg, benchmark_chiimg2, chiimg
TEST_FILE="test_chiimg.cpp"       # The test file name
TEST_EXEC="chiimg_benchmark_xrt"   # The name for the compiled executable

# Construct full test path
TEST_PATH="tests/${TEST_DIR}/${TEST_FILE}"

# Debugging info
echo "Running test configuration:"
echo "Test Directory: ${TEST_DIR}"
echo "Test File: ${TEST_FILE}"
echo "Full Test Path: ${TEST_PATH}"
echo "----------------------------------------"

############################## Environment Setup ##############################
# Setup environment
source /tools/Xilinx/Vitis/2023.1/settings64.sh
source /opt/xilinx/xrt/setup.sh

# Clean any previous builds
make cleanall

# Update MAKE variables for the specific test
export TEST_PATH=${TEST_PATH}
export TEST_EXEC=${TEST_EXEC}

# Build for sw_emu
make all TARGET=sw_emu PLATFORM=/opt/xilinx/platforms/xilinx_u280_gen3x16_xdma_1_202211_1/xilinx_u280_gen3x16_xdma_1_202211_1.xpfm

# Ensure emconfig is copied
if [ -f ./_x.sw_emu.xilinx_u280_gen3x16_xdma_1_202211_1/emconfig.json ]; then
    cp ./_x.sw_emu.xilinx_u280_gen3x16_xdma_1_202211_1/emconfig.json .
fi

# Set emulation mode
export XCL_EMULATION_MODE=sw_emu

# Run the benchmark
if [ -f ./${TEST_EXEC} ]; then
    ./${TEST_EXEC} -x build_dir.sw_emu.xilinx_u280_gen3x16_xdma_1_202211_1/chiimg_benchmark.xclbin
else
    echo "Error: executable '${TEST_EXEC}' not found"
    exit 1
fi

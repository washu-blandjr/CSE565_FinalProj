#
# Copyright 2019-2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License")

MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
PWD = $(shell readlink -f .)

########################## Platform Settings #######################
PLATFORM_BLOCKLIST += nodma
TARGET := sw_emu
PLATFORM ?= /opt/xilinx/platforms/xilinx_u280_gen3x16_xdma_1_202211_1/xilinx_u280_gen3x16_xdma_1_202211_1.xpfm

########################## Test Configuration #######################
TEST_PATH ?= tests/benchmark_chiimg/test_chiimg.cpp
TEST_EXEC ?= chiimg_benchmark_xrt

########################## Project Variables #######################
VPP_LDFLAGS :=
TEMP_DIR := ./_x.$(TARGET).xilinx_u280_gen3x16_xdma_1_202211_1
BUILD_DIR := ./build_dir.$(TARGET).xilinx_u280_gen3x16_xdma_1_202211_1
LINK_OUTPUT := $(BUILD_DIR)/chiimg_benchmark.link.xclbin
PACKAGE_OUT = ./package.$(TARGET)

# OpenCV paths
OPENCV_INCLUDE = /home/mrblandjr/Documents/local_installs/install/include/opencv4
OPENCV_LIB = /home/mrblandjr/Documents/local_installs/install/lib

# Vitis HLS paths
VITIS_HLS_INCLUDE = /tools/Xilinx/Vitis_HLS/2023.1/include

# V++ compiler flags
VPP_FLAGS += --save-temps
VPP_FLAGS += -I$(OPENCV_INCLUDE)
VPP_FLAGS += -I./src
VPP_FLAGS += -I$(VITIS_HLS_INCLUDE)

CMD_ARGS = -x $(BUILD_DIR)/chiimg_benchmark.xclbin

# Host compiler settings
CXXFLAGS += -I$(XILINX_XRT)/include 
CXXFLAGS += -I$(XILINX_VIVADO)/include 
CXXFLAGS += -I$(VITIS_HLS_INCLUDE)
CXXFLAGS += -I$(OPENCV_INCLUDE)
CXXFLAGS += -I./src
CXXFLAGS += -I.
CXXFLAGS += -Wall -O0 -g -std=c++17

# Libraries
LDFLAGS += -L$(XILINX_XRT)/lib -L$(OPENCV_LIB)
LDFLAGS += -pthread -lOpenCL -lxrt_coreutil
LDFLAGS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_features2d
LDFLAGS += -Wl,-rpath=$(OPENCV_LIB)

########################### Source Files ##########################
HOST_SRCS = $(TEST_PATH) \
            src/chiimg_wrapper.cpp \
            src/chiimg_kernel.cpp

KERNEL_SRCS = src/chiimg_kernel.cpp

EXECUTABLE = $(TEST_EXEC)
EMCONFIG_DIR = $(TEMP_DIR)

############################## Platform Check ##############################
.PHONY: check-platform
check-platform:
ifndef PLATFORM
    $(error PLATFORM is not set)
endif

############################## Targets ##############################
.PHONY: all clean cleanall docs emconfig
all: host xclbin emconfig

.PHONY: host
host: $(EXECUTABLE)
    @echo "OpenCV Include Path: $(OPENCV_INCLUDE)"
    @echo "OpenCV Lib Path: $(OPENCV_LIB)"

.PHONY: xclbin
xclbin: $(BUILD_DIR)/chiimg_benchmark.xclbin

# Host executable
$(EXECUTABLE): $(HOST_SRCS)
    g++ -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

# Kernel build rules
$(TEMP_DIR)/chiimg_kernel.xo: $(KERNEL_SRCS)
    mkdir -p $(TEMP_DIR)
    v++ -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k chiimg_kernel \
        --temp_dir $(TEMP_DIR) -I'$(<D)' -I$(OPENCV_INCLUDE) -o'$@' '$<'

$(BUILD_DIR)/chiimg_benchmark.xclbin: $(TEMP_DIR)/chiimg_kernel.xo
    mkdir -p $(BUILD_DIR)
    v++ -l $(VPP_FLAGS) $(VPP_LDFLAGS) -t $(TARGET) --platform $(PLATFORM) \
        --temp_dir $(TEMP_DIR) -o'$(LINK_OUTPUT)' $(+)
    v++ -p $(LINK_OUTPUT) $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) \
        --package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/chiimg_benchmark.xclbin

# Emulation config
emconfig: $(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
    mkdir -p $(EMCONFIG_DIR)
    emconfigutil --platform $(PLATFORM) --od $(EMCONFIG_DIR)

# Run targets
run: all
    cp -rf $(EMCONFIG_DIR)/emconfig.json .
    XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE) $(CMD_ARGS)

# Cleaning
clean:
    -$(RM) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*}
    -$(RM) -r profile_* TempConfig system_estimate.xtxt *.rpt *.csv
    -$(RM) src/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
    -$(RM) -r build_dir* package.*
    -$(RM) -r _x* *xclbin.run_summary qemu-memory-_* emulation _vimage pl* start_simulation.sh *.xclbin
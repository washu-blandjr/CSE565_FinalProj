

#+-------------------------------------------------------------------------------
# The following parameters are assigned with default values. These parameters can
# be overridden through the make command line
#+-------------------------------------------------------------------------------

PROFILE := no

#Generates profile summary report
ifeq ($(PROFILE), yes)
VPP_LDFLAGS += --profile.data all:all:all
endif

DEBUG := no

#Generates debug summary report
ifeq ($(DEBUG), yes)
VPP_LDFLAGS += --dk list_ports
endif

#Setting PLATFORM
ifeq ($(PLATFORM),)
ifneq ($(DEVICE),)
$(warning WARNING: DEVICE is deprecated in make command. Please use PLATFORM instead)
PLATFORM := $(DEVICE)
endif
endif

#Checks for XILINX_VITIS
check-vitis:
ifndef XILINX_VITIS
	$(error XILINX_VITIS variable is not set, please set correctly and rerun)
endif

#Checks for XILINX_XRT
check-xrt:
ifndef XILINX_XRT
	$(error XILINX_XRT variable is not set, please set correctly and rerun)
endif

check-device:
	@set -eu; \
	inallowlist=False; \
	inblocklist=False; \
	for dev in $(PLATFORM_ALLOWLIST); do \
	    if [[ $$(echo $(PLATFORM) | grep $$dev) == $(PLATFORM) ]]; then \
	        inallowlist=True; \
	    fi; \
	done; \
	for dev in $(PLATFORM_BLOCKLIST); do \
	    if [[ $$(echo $(PLATFORM) | grep $$dev) == $(PLATFORM) ]]; then \
	        inblocklist=True; \
	    fi; \
	done; \
	if [[ $$inblocklist == True ]]; then \
	    echo "Platform $(PLATFORM) is in block list."; \
	    exit 1; \
	else \
	    if [[ $$inallowlist == False ]]; then \
	        echo "Platform $(PLATFORM) is not in allowlist."; \
	        exit 1; \
	    fi; \
	fi

gen_run_app:
	rm -rf run_app.sh
	$(ECHO) 'export LD_LIBRARY_PATH=/mnt:/tmp:$$LD_LIBRARY_PATH' >> run_app.sh
	$(ECHO) 'export XILINX_XRT=/usr' >> run_app.sh
	$(ECHO) 'echo "INFO: Running application in hardware..."' >> run_app.sh
	$(ECHO) './$(EXECUTABLE) $(CMD_ARGS)' >> run_app.sh
	$(ECHO) 'return_code=$$?' >> run_app.sh
	$(ECHO) 'if [ $$return_code -ne 0 ]; then' >> run_app.sh
	$(ECHO) 'echo "ERROR: host run failed, RC=$$return_code"' >> run_app.sh
	$(ECHO) 'fi' >> run_app.sh
	$(ECHO) 'echo "INFO: host run completed."' >> run_app.sh

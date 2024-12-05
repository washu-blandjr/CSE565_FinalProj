#!/bin/bash
sed 's/sw_emu/hw/g' sw_emu_RUN.sh > hw_RUN.sh
sed 's/sw_emu/hw_emu/g' sw_emu_RUN.sh > hw_emu_RUN.sh


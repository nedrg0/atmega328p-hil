#!/usr/bin/env bash
# 
# Executes Makefiles for firmware and HIL testing

set -uo pipefail

# Paths
FIRMWARE_DIR="firmware/targets/atmega328"
HIL_TEST_DIR="hil"

RUN_MAKE_FLASH=1
RUN_MAKE_HIL_TEST=1

if [ "$RUN_MAKE_FLASH" -eq 1 ]; then
    echo "FLASHING TO MCU..."
    make -C "$FIRMWARE_DIR" flash
    echo "CLEANING OBJECT FILES.."
    make -C "$FIRMWARE_DIR" clean
fi 

if [ "$RUN_MAKE_HIL_TEST" -eq 1 ]; then
    make -C "$HIL_TEST_DIR" clean
    echo "RUNNING HIL TEST..."
    make -C "$HIL_TEST_DIR" run
    exit 0
fi


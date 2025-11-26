#!/bin/bash

# Input LAS parent directory location into the file
# with a backslash at the end
# DO NOT change the variable in this script unless that file name changes
CONFIG_FILENAME=Config.cfg

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )     # Where script is (should be Garage/build/scripts/
CONFIG_PATH="${SCRIPT_DIR}/${CONFIG_FILENAME}"

if [[ -e ${CONFIG_PATH} ]]; then
    source "${CONFIG_PATH}"
else
    echo "Configuration file not found at expected location: ${CONFIG_PATH}"
    exit 1
fi

cd ${SCRIPT_DIR}

cp -r ${LAS_DIR}bin/SDK/Debug/include ../../    # Get header Files
cp ${LAS_DIR}bin/SDK/Debug/libLAS-SDK-dev.so ../../lib/    # Get LAS-SDK-dev.so for compilation

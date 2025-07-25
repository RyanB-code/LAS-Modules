#!/bin/bash

# Input LAS parent directory location into the file
# with a backslash at the end
# DO NOT change the variable in this script unless that file name changes
LAS_LOCATION_FILE=las-directory.dat

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )     # Where script is (should be Garage/build/scripts/
LAS_DIR=$(cat ${SCRIPT_DIR}/${LAS_LOCATION_FILE})                                   # LAS directory

cd ${SCRIPT_DIR}

cp -r ${LAS_DIR}bin/SDK/Release/include ../../           # Get header Files
cp ${LAS_DIR}bin/SDK/Release/libLAS-SDK.so ../../lib/    # Get LAS-SDK.so for compilation

#!/bin/bash

# Input LAS parent directory location into the file
# with a backslash at the end
# DO NOT change the variable in this script unless that file name changes
LAS_LOCATION_FILE=las-directory.dat

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )     # Where script is (should be Garage/build/scripts/
CONFIG_FILE="${SCRIPT_DIR}/${LAS_LOCATION_FILE}"

if [[ -e ${CONFIG_FILE} ]]; then
    source "${CONFIG_FILE}"
else
    echo "Configuration file not found at expected location: ${CONFIG_FILE}" 
    exit 1
fi

cd ${SCRIPT_DIR}

cp ../../bin/*-dev.lasm ${LAS_DIR}bin/Environment/Debug/Module\ Libraries/    # Copy the module file to LAS debug area

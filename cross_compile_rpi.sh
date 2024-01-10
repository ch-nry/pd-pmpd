#!/bin/bash

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
SOURCE_FOLDER=${SCRIPT_DIR}
BUILD_FOLDER=${SOURCE_FOLDER}/build

cmake -S ${SOURCE_FOLDER} -B ${BUILD_FOLDER} -G Ninja -DCMAKE_TOOLCHAIN_FILE="${SOURCE_FOLDER}/arm-linux-gnueabihf.cmake" -DCMAKE_C_FLAGS="-marm"
cmake --build ${BUILD_FOLDER}
cmake --build ${BUILD_FOLDER} --target install

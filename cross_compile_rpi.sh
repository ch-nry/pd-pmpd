#!/bin/bash

SOURCE_FOLDER=/pmpd
BUILD_FOLDER=${SOURCE_FOLDER}/build

cmake -S ${SOURCE_FOLDER} -B ${BUILD_FOLDER} -G Ninja -DCMAKE_TOOLCHAIN_FILE="${SOURCE_FOLDER}/arm-linux-gnueabihf.cmake"
cmake --build ${BUILD_FOLDER}
cmake --build ${BUILD_FOLDER} --target install

#!/bin/bash

SPACK_PATH=/home/raphi/spack/opt

INCLUDE_PATHS=$(find $SPACK_PATH -type d -name "include")
LIB_PATHS=$(find $SPACK_PATH -type d -name "lib")

PATHS=
for NEW_PATH in $LIB_PATHS; do
    export PATHS=${NEW_PATH}${PATHS:+:$PATHS}
done
export LD_LIBRARY_PATH=${PATHS}${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export RPATH=${PATHS}${RPATH:+:$RPATH}

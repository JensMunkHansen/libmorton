#!/bin/bash
(
    source $HOME/github/emsdk/emsdk_env.sh
    emcmake cmake -DCMAKE_CXX_FLAGS="-matomics -mbulk-memory -pthread" -Bwasm -S. 
    cmake --build wasm
)

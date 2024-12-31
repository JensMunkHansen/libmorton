emcmake cmake -DCMAKE_CXX_FLAGS="--target=wasm64 -matomics -mbulk-memory -pthread" -B wasm -S . 
cmake --build wasm

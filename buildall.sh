# build release
./run_cmake.sh
pushd  bin_rel
make
popd

# build debug
./run_cmake_debug.sh
pushd  bin_debug
make
popd

# Todo also build the unit tests

#!/bin/sh
#rm *.proto
rm *.pb*

# wget -O tentacle-message.proto \
#   https://raw.githubusercontent.com/octoblu/tentacle-protocol-buffer/master/tentacle-message.proto
protoc tentacle-message.proto -otentacle-message.pb
python ~/Projects/Octoblu/nanopb/generator/nanopb_generator.py tentacle-message.pb

[ ! -d build ] && mkdir build
cd build && \
cmake  -Dtest=ON .. && \
make VERBOSE=1 && \
ctest -VV

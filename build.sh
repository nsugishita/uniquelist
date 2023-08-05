#!/bin/sh
#
# This is a script to demonstrate typical steps to build the library.

set -e

if [[ -z "${PREFIX}" ]]; then
  MY_PREFIX="/usr"
else
  MY_PREFIX="${PREFIX}"
fi

CMAKE="$MY_PREFIX/bin/cmake"
CXX="$MY_PREFIX/bin/g++"

rm -rf build
CXX="$CXX" $CMAKE -S . -B build
CXX="$CXX" $CMAKE --build build

pushd tests
python3 test.py
popd

echo "ok"

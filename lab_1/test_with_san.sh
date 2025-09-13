#!/bin/bash

./build_debug_asan.sh
echo "Results with address sanitizer"
./func_tests/scripts/func_tests.sh
./build_debug_msan.sh
echo "Results with memory sanitizer"
./func_tests/scripts/func_tests.sh
./build_debug_ubsan.sh
echo "Results with undefined behaviour sanitizer"
./func_tests/scripts/func_tests.sh

#!/bin/bash

./build_gcov.sh

./func_tests/scripts/func_tests.sh

for file in ./*.gcda; do
    gcov "$file"
done

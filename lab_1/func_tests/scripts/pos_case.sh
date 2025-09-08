#!/bin/bash

test_input="$1"
test_output="$2"
test_number=$(echo "$1" | grep -oP '[0-9]*')
app_output_file="app_output_pos_${test_number}"

if ! ./app.exe < "${test_input}" > "${app_output_file}"; then
    exit 1
fi

./func_tests/scripts/comparator.sh "${app_output_file}" "${test_output}"

exit $?

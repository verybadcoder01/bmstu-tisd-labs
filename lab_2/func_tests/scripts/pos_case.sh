#!/bin/bash

test_input="$1"
test_output="$2"
test_number=$(echo "$1" | grep -oP '[0-9]*')
app_output_file="app_output_pos_${test_number}"

exec_file="./app.exe"
if [[ ! -z "$3" ]]; then
    exec_file="./$3"
fi

if ! "${exec_file}" < "${test_input}" > "${app_output_file}"; then
    exit 1
fi

./func_tests/scripts/comparator.sh "${app_output_file}" "${test_output}"

exit $?

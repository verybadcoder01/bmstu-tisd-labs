#!/bin/bash

test_input="$1"
exec_file="./app.exe"
if [[ ! -z "$2" ]]; then
    exec_file="./$2"
fi


if ! "${exec_file}" < "${test_input}" > /dev/null; then
    exit 0
fi

exit 1

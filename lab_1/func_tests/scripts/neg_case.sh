#!/bin/bash

test_input="$1"

if ! ./app.exe < "${test_input}" > /dev/null; then
    exit 0
fi

exit 1

#!/bin/bash

regexp="-?[0-9]+(\.[0-9]*)?"

data_file1=$(less "$1" | grep -Eo "\\$regexp")
data_file2=$(less "$2" | grep -Eo "\\$regexp")

if [[ "${data_file1}" == "${data_file2}" ]]; then
    exit 0
fi

exit 1

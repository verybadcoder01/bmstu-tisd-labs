#!/bin/bash

regexp="Result:(.|\n)*"

data_file1=$(less "$1" | tr '\n' ' ' | grep -Po "$regexp")
data_file2=$(less "$2" | tr '\n' ' ' | grep -Po "$regexp")

if [[ "${data_file1}" == "${data_file2}" ]]; then
    exit 0
fi

exit 1

#!/bin/bash

gcc -c ./*.c -std=c99 -Wall -Werror -Wpedantic -Wextra -Wfloat-equal -Wfloat-conversion -Wvla
gcc ./*.c -o app.exe -lm

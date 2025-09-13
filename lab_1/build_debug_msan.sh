#!/bin/bash

clang -std=c99 -Wall -Werror -Wvla -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer -g ./*.c -o app.exe

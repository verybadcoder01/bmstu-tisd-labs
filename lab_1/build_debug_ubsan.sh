#!/bin/bash

clang -std=c99 -Wall -Werror -Wvla -fsanitize=undefined -fno-omit-frame-pointer -g ./*.c -o app.exe

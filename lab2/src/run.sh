#!/bin/bash

clang -o quicksort main.c -pthread

echo "Running system call trace..."
sudo dtruss ./quicksort 4

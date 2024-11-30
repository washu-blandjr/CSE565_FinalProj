#!/bin/bash
g++ -o test_findpeaks test_findpeaks.cpp ../../findpeaks.cpp -std=c++17 -Wall -Wextra -O2
./test_findpeaks

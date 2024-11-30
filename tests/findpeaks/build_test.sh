#!/bin/bash
g++ -o test_findpeaks test_findpeaks.cpp ../../findpeaks.cpp -std=c++17 -Wall -Wextra -O2
./test_findpeaks > output.txt
if diff output.txt golden.txt > /dev/null; then
    echo "!Output matches golden file!"
else
    echo "XXX Output differs from golden file XXX"
    diff output.txt golden
fi


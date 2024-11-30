#!/bin/bash
g++ -o test_chiimg test_chiimg.cpp ../../chiimg.cpp `pkg-config --cflags --libs opencv4`;
./test_chiimg > output.txt
if diff output.txt golden.txt > /dev/null; then
    echo "!Output matches golden file!"
else
    echo "XXX Output differs from golden file XXX"
    diff output.txt golden
fi

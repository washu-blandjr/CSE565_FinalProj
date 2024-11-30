#!/bin/bash
g++ -o test_chiimg test_chiimg.cpp ../../chiimg.cpp `pkg-config --cflags --libs opencv4`;
./test_chiimg

#!/bin/bash
g++ -fPIC -std=c++0x -ggdb -shared -ldl -lstdc++ -o fopen.so fopen.cpp
gcc proof.cpp -o proof

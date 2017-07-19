#!/bin/bash
set -x
g++ -fPIC -std=c++0x -ggdb -shared -ldl -lstdc++ -o inspectio.so  inspectio11.cpp


#!/bin/bash
g++ -fPIC -std=c++0x -ggdb -shared -ldl -lstdc++ -o inspectio.so  inspectio10.cpp


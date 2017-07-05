#!/bin/bash
CC=clang++-3.5 
#CC=g++
set -x
$CC -std=c++0x pcp.cpp -o pcp

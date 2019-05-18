#!/bin/bash
set -x
gcc -o joy joy4.cpp  -lSDL2 -lm -lSDL2_image

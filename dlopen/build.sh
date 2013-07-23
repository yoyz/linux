#!/bin/bash
set -x
gcc     main.c   -o main -ldl
gcc -shared -fPIC -o libtiti.so -g -Wall libtiti.c
gcc -shared -fPIC -o libtoto.so -g -Wall libtoto.c

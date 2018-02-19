#!/bin/bash
nasm -f elf64 hello64.s
ld hello64.o

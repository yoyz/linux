#!/bin/bash
set -x
gcc -lext2fs -lcom_err openandlist.c  -o openandlist

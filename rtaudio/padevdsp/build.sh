#!/bin/bash
set -x 
g++ -I.. -D__LINUX_PULSE__  padevdsp.cpp ../RtAudio.cpp -o padevdsp -lpulse-simple -lpulse    -lpthread
g++ square_gen.cpp   -o square_gen
g++ square_recv.cpp  -o square_recv

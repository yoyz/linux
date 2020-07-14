#!/bin/bash
set -x 
g++ -I.. -D__LINUX_PULSE__  square.cpp ../RtAudio.cpp -o square -lpulse-simple -lpulse    -lpthread

g++ -fPIC -std=c++0x -ldl -shared inspectio12.cpp -o inspectio.so
export    INSPECTIO_DUMP=$PWD/LOG/
mkdir -p $INSPECTIO_DUMP
export    INSPECTIO_PATH=$(pwd)
rm $INSPECTIO_PATH/LOG/* ; export LD_PRELOAD=$INSPECTIO_PATH/inspectio.so:$LD_PRELOAD ; xterm ; unset LD_PRELOAD

#!/bin/bash

set -e

# This file must be executed from the folder HybridLearner/build but before executing please perform the TODO below.

source ../config
echo MATLAB_ROOT=$MATLAB_ROOT

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib:/usr/local/lib:/usr/lib/x86_64-linux-gnu:$MATLAB_ROOT/bin/glnxa64:$MATLAB_ROOT/bin/glnxa64/builtins/sl_main/:$MATLAB_ROOT/extern/bin/glnxa64:$MATLAB_ROOT/extern/lib/glnxa64:$MATLAB_ROOT/sys/os/glnxa64
export PATH=$PATH:/usr/lib/x86_64-linux-gnu/glib-2.0/:$MATLAB_ROOT/bin
export DISPLAY=
export LANG=en

FIFODIR=$(mktemp -d)
FIFO=$FIFODIR/fifo
mkfifo $FIFO

echo Launching MATLAB...
tail -f $FIFO | matlab -nodisplay -r "matlab.engine.shareEngine" &
PID=$!
echo Launched MATLAB FIFO=$FIFO PID=$PID

close_matlab ()
{
    if [ -e $FIFO ]; then
	echo Closing MATLAB...
	echo Sending quit to $FIFO
	echo quit > $FIFO
    fi
    echo Killing MATLAB $PID
    kill -KILL $PID
    rm -rf $FIFODIR
}

trap close_matlab ERR INT

sleep 5

# Execute the shell script for learning the benchmark models
# This took approximately 2 hours on Intel i9 CPU 2.4 GHz with 32 GB RAM
./run_results_main_learn_models.sh

echo "Done Learning Models"

# Before executing this, we assume the script run_results_main_learn_models is already executed and models are learned and copied in the respective folders.
matlab -nodisplay -nosplash -batch "run('../examples/ball/BeforeAnnotation/call_run_ball.m')"
matlab -nodisplay -nosplash -batch "run('../examples/ball/AfterAnnotation/call_run_ball.m')"
# matlab -nodisplay -nosplash -r "cd('../examples'); run('run_main.m'); exit;"
echo "Done generating output Trajectories"

# Before executing this, we assume the scripts (1) run_results_main_learn_models and (2) run_main.m
# have already been excuted and models are learned and output trajectories are generated and copied in the respective folders.
python3 ../examples/computeDTW_dist_main.py
echo "Done creating summary_results.csv; DTW distances between learned and Test Trajectories"

close_matlab

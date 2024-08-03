#!/bin/bash

set -e

# Script file to reproduce the results presented in the paper "Learning nonlinear hybrid automata from input–output time-series data"
# This file must be executed from the folder HybridLearner/build



## # **** Switched Oscillator, small size *****
## 
## ID=oscillator4/BeforeAnnotation
## echo Executing $ID
## OUTDIR=_result/$ID
## mkdir -p $OUTDIR || true
## 
## CMD='./HybridLearner --engine learn-ha-loop --output-directory $OUTDIR --output-file xxx.txt --simulink-model-file ../src/test_cases/engine/learn_ha_loop/oscillator.slx --variable-category "x:output, y:output"  --simu-init-size 4 --initial-value "x>=0.01 & x<=0.09 & y>=0.01 & y<=0.09" --time-horizon 10 --sampling-time 0.01 --modes 4 --degree 1 --boundary-degree 1 --segment-relative-error 0.1 --segment-relative-fine-error 0.01  --precision-equivalence 100.0 --max-traces 3 --max-stoptime 20 --invariant 2  --cluster-algo dtw  --correlation-threshold 0.89 --distance-threshold 1.0   --max-generate-trace-size 1024  --ode-speedup 50 --filter-last-segment 1 --solver-type fixed --ode-solver FixedStepAuto'
## 
## # eval required to evaluate variables and quoted arguments correctly
## eval $CMD \
##      > $OUTDIR/log.txt
## 
## exit 0

# **** Switched Oscillator *****

ID=oscillator/BeforeAnnotation
echo Executing $ID
OUTDIR=_result/$ID
mkdir -p $OUTDIR || true

CMD='./HybridLearner --engine learn-ha-loop --output-directory $OUTDIR --output-file xxx.txt --simulink-model-file ../src/test_cases/engine/learn_ha_loop/oscillator.slx --variable-category "x:output, y:output"  --simu-init-size 64 --initial-value "x>=0.01 & x<=0.09 & y>=0.01 & y<=0.09" --time-horizon 10 --sampling-time 0.01 --modes 4 --degree 1 --boundary-degree 1 --segment-relative-error 0.1 --segment-relative-fine-error 0.01  --precision-equivalence 100.0 --max-traces 3 --max-stoptime 20 --invariant 2  --cluster-algo dtw  --correlation-threshold 0.89 --distance-threshold 1.0   --max-generate-trace-size 1024  --ode-speedup 50 --filter-last-segment 1 --solver-type fixed --ode-solver FixedStepAuto'

# eval required to evaluate variables and quoted arguments correctly
eval $CMD \
     > $OUTDIR/log.txt

ID=oscillator/AfterAnnotation
echo Executing $ID
OUTDIR=_result/$ID
mkdir -p $OUTDIR || true

eval $CMD \
     --variable-types 'x0=t1,x1=t1' \
     > $OUTDIR/log.txt

# **** Bouncing Ball *****

ID=bball/BeforeAnnotation
echo Executing $ID
OUTDIR=_result/$ID
mkdir -p $OUTDIR || true

CMD='./HybridLearner --engine learn-ha-loop --output-directory $OUTDIR --output-file xxx.txt --simulink-model-file ../src/test_cases/engine/learn_ha_loop/ex_sldemo_bounce_Input.slx --variable-category "u:input, x:output, v:output"  --simu-init-size 64 --initial-value "u>=-9.5 & u<=-9.9 & x>=10.2 & x<=10.5 & v>=15 & v<=15"  --input-signal-type "u=linear" --numberOf-control-points "u=4"  --time-horizon 13 --sampling-time 0.001 --modes 1 --degree 1 --boundary-degree 1 --segment-relative-error 0.1  --segment-relative-fine-error 0.01 --precision-equivalence 50.0 --max-traces 1 --max-stoptime 20 --invariant 2  --cluster-algo dtw  --correlation-threshold 0.8 --distance-threshold 9.0  --fixed-interval-data 0 --filter-last-segment 1  --max-generate-trace-size 1024  --ode-speedup 50 --solver-type fixed --ode-solver FixedStepAuto'

eval $CMD \
     > $OUTDIR/log.txt

ID=bball/AfterAnnotation
echo Executing $ID
OUTDIR=_result/$ID
mkdir -p $OUTDIR || true

eval $CMD \
     --variable-types 'x0=t1,x1=t3' --t3-values 'x1=0' \
     > $OUTDIR/log.txt

# **** Two Tanks *****

ID=twoTanks/BeforeAnnotation
echo Executing $ID
OUTDIR=_result/$ID
mkdir -p $OUTDIR || true

CMD='./HybridLearner --engine learn-ha-loop --output-directory $OUTDIR --output-file xxx.txt  --simulink-model-file ../src/test_cases/engine/learn_ha_loop/twoTank.slx --variable-category "u:input, x1:output, x2:output" --simu-init-size 64 --initial-value "u>=-0.1 & u<=0.1 & x1>=1.2 & x1<=1.2 & x2>=1 & x2<=1"  --input-signal-type "u=linear" --numberOf-control-points "u=2"  --time-horizon 9.3 --sampling-time 0.001 --modes 4 --degree 1 --boundary-degree 1 --segment-relative-error 0.01 --segment-relative-fine-error 0.01  --precision-equivalence 10.5 --max-traces 1 --max-stoptime 20  --invariant 2 --cluster-algo dtw  --correlation-threshold 0.7 --distance-threshold 1.5  --max-generate-trace-size 1024  --filter-last-segment 1 --ode-speedup 50 --solver-type fixed --ode-solver FixedStepAuto'

eval $CMD \
     > $OUTDIR/log.txt

ID=twoTanks/AfterAnnotation
echo Executing $ID
OUTDIR=_result/$ID
mkdir -p $OUTDIR || true

eval $CMD \
     --variable-types 'x0=t1,x1=t1,x2=t1' \
     > $OUTDIR/log.txt

# **** Cell Model *****

ID=cell/BeforeAnnotation
echo Executing $ID
OUTDIR=_result/$ID
mkdir -p $OUTDIR || true

CMD='./HybridLearner --engine learn-ha-loop --output-directory $OUTDIR --output-file cellModel_64.txt --simulink-model-file ../src/test_cases/engine/learn_ha_loop/cell/excitable_cell.slx --variable-category "x:output" --simu-init-size 64 --initial-value "x>=-76 & x<=-74" --time-horizon 500 --sampling-time 0.01 --modes 4 --degree 1 --boundary-degree 1 --segment-relative-error 0.01 --segment-relative-fine-error 0.01  --precision-equivalence 500.0 --max-traces 2 --max-stoptime 20  --invariant 2  --cluster-algo dtw  --correlation-threshold 0.92 --distance-threshold 1.0 --ode-speedup 3 --max-generate-trace-size 1024 --filter-last-segment 1 --solver-type fixed --ode-solver FixedStepAuto'

eval $CMD \
     > $OUTDIR/log.txt

ID=cell/AfterAnnotation
echo Executing $ID
OUTDIR=_result/$ID
mkdir -p $OUTDIR || true

eval $CMD \
     --variable-types 'x0=t1' \
     > $OUTDIR/log.txt

# **** Engine Timing System *****. Here it first learns HA as a plain text-file from trajectories as input file, then transform txt to SLX model

ID=engine/BeforeAnnotation
echo Executing $ID
OUTDIR=_result/$ID
mkdir -p $OUTDIR || true

CMD='./HybridLearner --engine learn-ha --output-directory $OUTDIR --output-file engineTiming_64.txt  --simu-trace-file ../src/test_cases/engine/learn_ha/enginetiming/dataBBC/engine_64.txt  --variable-category "throttle:input, torque:input, engineSpeed:output" --modes 20 --degree 1 --boundary-degree 1 --segment-relative-error 0.99  --segment-relative-fine-error 0.01   --invariant 2  --cluster-algo dtw  --correlation-threshold 0.9 --distance-threshold 1000 --ode-speedup 100  --fixed-interval-data 0 --filter-last-segment 0 --lmm-step-size 5'

eval $CMD \
     >  $OUTDIR/log.txt

# I beleve it fails since there is no engineTiming_64.txt
CMD2='./HybridLearner --engine txt2slx --variable-category "x0:input, x1:input, x2:output" --model-file engineTiming_64.txt --time-horizon 10 --sampling-time 0.01 --invariant 2 --input-signal-type "x0=fixed-step & x1=fixed-step" --degree 1 --ode-solver ode45 --fixed-interval-data 0  --numberOf-control-points "x0=3 & x1=3"  --initial-value "x0>=2 & x0<=2 & x1>=24 & x1<=24 & x2=2000" # we do not actually use this parameters for Testing instead use the Test Set input signal'

eval $CMD2 \
     >> $OUTDIR/log.txt

ID=engine/AfterAnnotation
echo Executing $ID
OUTDIR=_result/$ID
mkdir -p $OUTDIR || true

eval $CMD \
     --variable-types 'x2=t1' \
     > $OUTDIR/log.txt

eval $CMD2 \
     >> $OUTDIR/log.txt

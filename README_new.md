# HybridLearner

## Requirements

- CMake 3.10
- C++ 17 compiler
- Boost
- MATLAB 2022b, installed at /usr/local/MATLAB/R2022b
- Python 3.10
- learnHA submodule

## Submodule build

Installation of Python packages for `src/learnHA`:

```
$ cd src/learnHA
$ python3 -m ensurepip --upgrade
$ pip install pipenv
$ pipenv --python 3.10
$ pipenv install
```

### Test

```
$ sh ./run_test
...
OK
```

## Build

```
$ git submodule update
$ cmake .
$ make
```

### Test

```
$ cd build
$ ./run_results_main_learn_models.sh
# results are stored under build/_result
```

Test results:

```
$ ls _result/bball/BeforeAnnotation
```

- `log.txt`: Log
- `original_model_simulate.m`: MATLAB script to simulate the original model 
- `original_model_traces_for_learning.txt`: Original model simulation result given to learnHA
- `learned_HA.txt`: Hybrid Automaton obtained by learnHA
- `learned_HA.json`: Hybrid Automaton obtained by learnHA, in a new format
- `generate_learned_model0_slx.m`: MATLAB script to compile the Hybrid Automaton
- `learned_model0.slx`: Compiled Hybrid Automaton as a SLX model
- `learned_model_simulate0.m`: MATLAB script to simulate the learned model
- `learned_model_traces0.txt`: Learned model simulation result
- `original_model_traces.txt`: Original model simulation result for the comparison with the learned model simulation

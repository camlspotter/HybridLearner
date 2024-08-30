cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(HybridLearner)

file(GLOB_RECURSE sources src/*/*.cpp src/*/*.h)

add_executable(HybridLearner ${sources})

set_target_properties(HybridLearner PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY build
)

target_include_directories(HybridLearner PRIVATE /usr/local/MATLAB/R2022b/extern/include)

link_directories(
  /usr/local/MATLAB/R2022b/bin/glnxa64
  /usr/local/MATLAB/R2022b/extern/bin/glnxa64
  )

target_link_libraries(HybridLearner PRIVATE
  pthread
  boost_program_options
  boost_system
  boost_timer
  boost_chrono
  m
  /usr/local/MATLAB/R2022b/bin/glnxa64/libmx.so
  /usr/local/MATLAB/R2022b/bin/glnxa64/libmat.so
  /usr/local/MATLAB/R2022b/bin/glnxa64/libMatlabDataArray.so
  /usr/local/MATLAB/R2022b/bin/glnxa64/libeng.so
  /usr/local/MATLAB/R2022b/extern/bin/glnxa64/libMatlabEngine.so
)

#ifndef UTILITIES_MATLAB_H
#define UTILITIES_MATLAB_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
// #include <MatlabDataArray.hpp>
#include <MatlabEngine.hpp>
#pragma GCC diagnostic pop

#endif

#define MATLAB_EVAL(ep,cmd) do{std::cout << "MATLAB eval: " << cmd << std::endl; ep->eval(convertUTF8StringToUTF16String(cmd));}while(0)
#define MATLAB_RUN(ep,fn) do{std::string command = "run(\"" + fn + "\")"; MATLAB_EVAL(ep,command);}while(0)

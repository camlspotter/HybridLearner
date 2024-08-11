#ifndef UTILITIES_SYSTEM_H
#define UTILITIES_SYSTEM_H

#include <iostream>

#define system_must_succeed(c) { std::cout << "COMMAND " << c << std::endl; if( system(c.c_str()) != 0 ){ throw std::runtime_error("Command failed: " + c); } }

void system_copy_file(const std::filesystem::path& f1, const std::filesystem::path& f2);

void system_append_file(const std::filesystem::path& f1, const std::filesystem::path& f2);

#endif

#ifndef UTILITIES_FILESYSTEM_H_
#define UTILITIES_FILESYSTEM_H_

#include <filesystem>

std::string getcwd();
std::string concat_path(std::string a, std::string b);
std::string abspath(std::string path);
std::string dirname(std::string path);
std::string basename(std::string path);
std::string basename_without_ext(std::string path);
bool is_absolute_path(std::string path);

#endif


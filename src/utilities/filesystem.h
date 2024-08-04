#ifndef UTILITIES_FILESYSTEM_H_
#define UTILITIES_FILESYSTEM_H_

#include <filesystem>

namespace fs = std::filesystem;

fs::path getcwd();
fs::path concat_path(fs::path a, fs::path b);
fs::path abspath(fs::path path);
fs::path dirname(fs::path path);
fs::path basename(fs::path path);
fs::path basename_without_ext(fs::path path);
bool is_absolute_path(fs::path path);

#endif


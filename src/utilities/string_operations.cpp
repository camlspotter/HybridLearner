/*
 * string_operations.cpp
 *
 *  Created on: 27-Feb-2023
 *      Author: amit
 */


#include <filesystem>

namespace fs = std::filesystem;

std::string concat_path(std::string a, std::string b)
{
    return (fs::path(a) / fs::path(b)).string();
}

std::string dirname(std::string path)
{
    return fs::path(path).parent_path().string();
}

std::string basename(std::string path)
{
    return fs::path(path).filename().string();
}

std::string basename_without_ext(std::string path)
{
    return fs::path(path).stem().string();
}

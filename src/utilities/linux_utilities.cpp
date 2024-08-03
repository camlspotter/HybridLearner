/*
 * linux_utilities.cpp
 *
 *  Created on: 07-Nov-2021
 *      Author: amit
 */

#include <filesystem>
#include "string_operations.h"

namespace fs = std::filesystem;

std::string getcwd()
{
    return fs::current_path().string();
}

std::string abspath(std::string path)
{
    return concat_path(getcwd(), path);
}

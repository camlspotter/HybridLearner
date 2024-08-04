#include <filesystem>

namespace fs = std::filesystem;

std::string getcwd()
{
    return fs::current_path().string();
}

std::string concat_path(std::string a, std::string b)
{
    return (fs::path(a) / fs::path(b)).string();
}

std::string abspath(std::string path)
{
    return concat_path(getcwd(), path);
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

bool is_absolute_path(std::string path)
{
    return fs::path(path).is_absolute();
}

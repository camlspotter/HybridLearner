#include <filesystem>

namespace fs = std::filesystem;

fs::path getcwd()
{
    return fs::current_path();
}

fs::path concat_path(fs::path a, fs::path b)
{
    return a / b;
}

fs::path abspath(fs::path path)
{
    return concat_path(getcwd(), path);
}

fs::path dirname(fs::path path)
{
    return path.parent_path();
}

fs::path basename(fs::path path)
{
    return path.filename();
}

fs::path basename_without_ext(fs::path path)
{
    return path.stem();
}

bool is_absolute_path(fs::path path)
{
    return path.is_absolute();
}

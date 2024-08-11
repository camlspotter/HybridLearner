#include <filesystem>
#include "system.h"

void system_copy_file(const std::filesystem::path& f1, const std::filesystem::path& f2)
{
    std::string cmd="cp ";
	cmd.append(f1);
	cmd.append(" ");
	cmd.append(f2);
	system_must_succeed(cmd);
}

void system_append_file(const std::filesystem::path& f1, const std::filesystem::path& f2)
{
    std::string cmd="cat ";
	cmd.append(f1);
	cmd.append(" >> ");
	cmd.append(f2);
	system_must_succeed(cmd);
}


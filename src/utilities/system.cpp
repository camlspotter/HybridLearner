#include <filesystem.h>
#include "system.h"

void copy_file(std::filesystem::path& f1, std::filesystem::path& f2)
{
    std::string cmd="cp ";
	cmd.append(f1);
	cmd.append(" ");
	cmd.append(f2);
	system_must_succeed(cmd);
}

void append_file(std::filesystem::path& f1, std::filesystem::path& f2)
{
    std::string cmd="cat ";
	cmd.append(f1);
	cmd.append(" >> ");
	cmd.append(f2);
	system_must_succeed(cmd);
}


#ifndef UTILITIES_SYSTEM_H
#define UTILITIES_SYSTEM_H

#define system_must_succeed(c) { cout << "COMMAND " << c << endl; if( system(c.c_str()) != 0 ){ throw std::runtime_error("Command failed: " + c); } }

void copy_file(std::filesystem::path& f1, std::filesystem::path& f2);

void append_file(std::filesystem::path& f1, std::filesystem::path& f2);

#endif

#ifndef UTILITIES_SYSTEM_H
#define UTILITIES_SYSTEM_H

#define system_must_succeed(c) if( system(c.c_str()) != 0 ){ throw std::runtime_error("Command failed: " + c); }

#endif

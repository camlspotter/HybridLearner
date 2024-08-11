#include <iostream>
#include <string>
#include <cstdio>   // vsnprintf
#include <stdexcept> // std::runtime_error
#include <cstdarg>  // va_list, va_start, va_end

std::string formatString(const char* format, ...) {
    int size = 256;
    std::string result;
    
    while (true) {
        result.resize(size);
        
        va_list args;
        va_start(args, format);
        
        int n = vsnprintf(result.data(), size, format, args);
        
        va_end(args);

        if (n >= 0 && n < size) {
            result.resize(n);
            return result;
        }

        if (n >= 0) {
            size = n + 1;
        } else {
            // For old glibc. The output is truncated, but the required size is unknown
            size *= 2;
        }
    }
}

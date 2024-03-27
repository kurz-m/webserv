#ifndef __HTTP_HPP__
#define __HTTP_HPP__

#include <string>

enum method_e {
    GET = (1 << 0),
    POST = (1 << 1),
    PUT = (1 << 2),
    DELETE = (1 << 3),
    UNKNOWN = (1 << 4),
};

method_e method_to_enum(std::string& method);
const std::string print_method(const int& method);

#endif

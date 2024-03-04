#include "HTTP.hpp"
#include <algorithm>

method_e method_to_enum(std::string& method)
{
    const std::string method_arr[4] = {
        "GET",
        "POST",
        "PUT",
        "DELETE"
    };

    for (size_t i = 0; i < 4; ++i) {
        if (method == method_arr[i]) {
            return static_cast<method_e>(1 << i);
        }
    }
    return UNKNOWN;
}
#include <algorithm>
#include <iostream>

#include "HTTP.hpp"

method_e method_to_enum(std::string &method) {
  const std::string method_arr[8] = {"GET",  "POST",    "PUT",    "DELETE",
                                     "HEAD", "CONNECT", "OPTION", "TRACE"};

  for (size_t i = 0; i < 8; ++i) {
    if (method == method_arr[i]) {
      return static_cast<method_e>(1 << i);
    }
  }
  return UNKNOWN;
}

const std::string print_method(const int &method) {
  if (method & GET) {
    return "GET";
  }
  if (method & POST) {
    return "POST";
  }
  if (method & PUT) {
    return "PUT";
  }
  if (method & DELETE) {
    return "DELETE";
  }
  return "NOT IMPLEMENTED";
}

#include "HTTP.hpp"
#include <algorithm>
#include <iostream>

method_e method_to_enum(std::string &method) {
  const std::string method_arr[4] = {"GET", "POST", "PUT", "DELETE"};

  for (size_t i = 0; i < 4; ++i) {
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

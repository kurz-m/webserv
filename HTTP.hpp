#ifndef __HTTP_HPP
#define __HTTP_HPP

#include <string>

enum method_e {
  GET = (1 << 0),       /**< Represents the http get method*/
  POST = (1 << 1),      /**< Represents the http post method*/
  PUT = (1 << 2),       /**< Represents the http put method*/
  DELETE = (1 << 3),    /**< Represents the http delete method*/
  UNKNOWN = (1 << 4),   /**< Represents an unknow http method*/
  FORBIDDEN = (1 << 5), /**< Represents forbidden method in current location*/
};

method_e method_to_enum(std::string &method);
const std::string print_method(const int &method);

#endif /* HTTP.hpp */

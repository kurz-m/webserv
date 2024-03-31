#ifndef __HTTP_HPP
#define __HTTP_HPP

#include <string>

/**
 * Header file for http methods.
 *
 * This file serves as a helper to implement the different methods
 * specified in the RFC 9110.
 */
enum method_e {
  GET = (1 << 0),       /**< Represents the http get method*/
  POST = (1 << 1),      /**< Represents the http post method*/
  PUT = (1 << 2),       /**< Represents the http put method*/
  DELETE = (1 << 3),    /**< Represents the http delete method*/
  UNKNOWN = (1 << 4),   /**< Represents an unknow http method*/
  FORBIDDEN = (1 << 5), /**< Represents forbidden method in current location*/
};

/**
 * Function to convert a methond (std::string) to an enum.
 *
 * This function is used as a helper function for converting the
 * given std::string to the method_e enum.
 *
 * \param std::string Reference to a method as string.
 * \return Enum for the parsed http method.
 */
method_e method_to_enum(std::string &method);

/**
 * Helper function to print the enum as a string.
 *
 * The purpose of the function is only to print the given enum.
 *
 * \param method An enum representing a http method.
 * \return The string representation of the http method.
 *
 */
const std::string print_method(const int &method);

#endif /* HTTP.hpp */

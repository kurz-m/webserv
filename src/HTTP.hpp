#ifndef __HTTP_HPP
#define __HTTP_HPP

#include <string>

/**
 * Enum for specifying different http methods.
 *
 * This enum defines the different http methods that are taken from RFC 9110 and
 * implemented in this specific webserver.
 */
enum method_e {
  GET = (1 << 0),       /**< Represents the http get method. */
  POST = (1 << 1),      /**< Represents the http post method. */
  PUT = (1 << 2),       /**< Represents the http put method. */
  DELETE = (1 << 3),    /**< Represents the http delete method. */
  HEAD = (1 << 4),      /**< Represents the http head method. */
  CONNECT = (1 << 5),   /**< Represents the http connect method. */
  OPTION = (1 << 6),   /**< Represents the http option method. */
  TRACE = (1 << 7),   /**< Represents the http trace method. */
  UNKNOWN = (1 << 8),   /**< Represents an unknown http method. */
  FORBIDDEN = (1 << 9), /**< Represents forbidden method in current location. */
};

/**
 * Public function to convert a methond (std::string) to an enum.
 *
 * This function is used as a helper function for converting the
 * given std::string to the method_e enum.
 *
 * \param std::string Reference to a method as string.
 * \return Enum for the parsed http method.
 */
method_e method_to_enum(std::string &method);

/**
 * Public helper function to print the enum as a string.
 *
 * The purpose of the function is only to print the given enum.
 *
 * \param method An enum representing a http method.
 * \return The string representation of the http method.
 *
 */
const std::string print_method(const int &method);

#endif /* HTTP.hpp */

#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include "Token.hpp"

#include <string>
#include <vector>

struct Setting {
  enum Type { UNSET, STRING, INT } type;
  Token::token_type_t name;
  std::string str_val;
  int int_val;
};

struct RouteBlock {
  std::string path;
  std::vector<Setting> settings;

  Setting find(const std::string&);
};

struct ServerBlock {
  std::vector<Setting> settings;
  std::vector<RouteBlock> routes;

  Setting find(const std::string&);
};

struct HttpBlock {
  std::vector<Setting> settings;
  std::vector<ServerBlock> servers;

  Setting find(const std::string&);
};

#endif // !__SETTINGS_HPP__

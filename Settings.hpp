#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include "Token.hpp"

#include <string>
#include <vector>
#include "Token.hpp"

class NotFoundError : public std::exception {
public:
  const char *what() const throw();
};

struct Setting {
  enum Type { UNSET, STRING, INT } type;
  Token::token_type_t name;
  std::string str_val;
  int int_val;
};

struct RouteBlock {
  std::string path;
  std::vector<Setting> settings;

  const Setting find(Token::token_type_t) const;
};

struct ServerBlock {
  std::vector<Setting> settings;
  std::vector<RouteBlock> routes;

  const Setting find(Token::token_type_t) const;
  const RouteBlock& find(const std::string &) const;
};

struct HttpBlock {
  std::vector<Setting> settings;
  std::vector<ServerBlock> servers;

  const Setting find(Token::token_type_t) const;
};

#endif // !__SETTINGS_HPP__

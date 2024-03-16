#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

#include "Token.hpp"

#include "Token.hpp"
#include <string>
#include <vector>

struct HttpBlock;
struct ServerBlock;

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

typedef struct RouteBlock {
  RouteBlock(const ServerBlock &server);

  const Setting find(Token::token_type_t) const;

  std::string path;
  std::vector<Setting> settings;

  int allow;
  int autoindex;
  std::string index;
} RouteBlock;

typedef struct ServerBlock {
  ServerBlock(const HttpBlock &http);

  const Setting find(Token::token_type_t) const;
  const RouteBlock *find(const std::string &uri) const;

  std::vector<Setting> settings;
  std::vector<RouteBlock> routes;

  int allow;
  int autoindex;
  int client_max_body_size;
  std::string default_type;
  std::string index;
  int keepalive_timeout;
  std::string listen;
  std::string root;
  std::string server_name;
} ServerBlock;

typedef struct HttpBlock {
  HttpBlock();

  const Setting find(Token::token_type_t) const;

  std::vector<Setting> settings;
  std::vector<ServerBlock> servers;

  int client_max_body_size;
  std::string default_type;
  int keepalive_timeout;
  std::string root;

} HttpBlock;

#endif // !__SETTINGS_HPP__

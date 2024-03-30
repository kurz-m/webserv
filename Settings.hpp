#ifndef __SETTINGS_HPP
#define __SETTINGS_HPP

#include <string>
#include <vector>

#include "Token.hpp"

struct HttpBlock;
struct ServerBlock;

class NotFoundError : public std::exception {
public:
  const char *what() const throw();
};

typedef struct RouteBlock {
  RouteBlock(const ServerBlock &server);

  std::string path;

  int allow;
  int autoindex;
  std::string index;
} RouteBlock;

typedef struct ServerBlock {
  ServerBlock(const HttpBlock &http);

  const RouteBlock *find(const std::string &uri) const;

  std::vector<RouteBlock> routes;

  int allow;
  int autoindex;
  size_t client_max_body_size;
  std::string default_type;
  std::string index;
  int keepalive_timeout;
  std::string listen;
  std::string root;
  std::string server_name;
} ServerBlock;

typedef struct HttpBlock {
  HttpBlock();

  std::vector<ServerBlock> servers;

  int client_max_body_size;
  std::string default_type;
  int keepalive_timeout;
  std::string root;

} HttpBlock;

#endif /* Settings.hpp */

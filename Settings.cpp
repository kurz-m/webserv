#include "Settings.hpp"

RouteBlock::RouteBlock(const ServerBlock &server)
    : path(), allow(server.allow), autoindex(server.autoindex),
      index(server.index) {}

ServerBlock::ServerBlock(const HttpBlock &http)
    : routes(), allow(0), autoindex(0),
      client_max_body_size(http.client_max_body_size),
      default_type(http.default_type), index(),
      keepalive_timeout(http.keepalive_timeout), listen(), root(http.root),
      server_name() {}

HttpBlock::HttpBlock()
    : client_max_body_size(1024), default_type(), keepalive_timeout(10),
      root() {}

const char *NotFoundError::what() const throw() {
  return "Requested Setting or Route not found!";
}

const Setting RouteBlock::find(Token::token_type_t name) const {
  std::vector<Setting>::const_iterator it;
  for (it = settings.begin(); it != settings.end(); ++it) {
    if (it->name == name) {
      return *it;
    }
  }
  throw NotFoundError();
}

const Setting ServerBlock::find(Token::token_type_t name) const {
  std::vector<Setting>::const_iterator it;
  for (it = settings.begin(); it != settings.end(); ++it) {
    if (it->name == name) {
      return *it;
    }
  }
  throw NotFoundError();
}

const RouteBlock *ServerBlock::find(const std::string &uri) const {
  const RouteBlock *ret = NULL;
  std::vector<RouteBlock>::const_iterator it;
  for (it = routes.begin(); it != routes.end(); ++it) {
    if (uri.find(it->path) != std::string::npos) {
      if (!ret) {
        ret = &(*it);
      } else if (it->path.length() > ret->path.length()) {
        ret = &(*it);
      }
    }
  }
  return ret;
}

const Setting HttpBlock::find(Token::token_type_t name) const {
  std::vector<Setting>::const_iterator it;
  for (it = settings.begin(); it != settings.end(); ++it) {
    if (it->name == name) {
      return *it;
    }
  }
  throw NotFoundError();
}

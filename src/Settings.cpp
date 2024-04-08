#include "Settings.hpp"

RouteBlock::RouteBlock(const ServerBlock &server)
    : path(), allow(server.allow), autoindex(server.autoindex),
      index(server.index), redir() {}

ServerBlock::ServerBlock(const HttpBlock &http)
    : routes(), allow(0), autoindex(0),
      client_max_body_size(http.client_max_body_size),
      default_type(http.default_type), index(),
      keepalive_timeout(http.keepalive_timeout), listen(), root(http.root),
      server_name() {}

HttpBlock::HttpBlock()
    : client_max_body_size(1024), default_type("text/html"),
      keepalive_timeout(DEFAULT_TIMEOUT), root("./data/www") {}

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

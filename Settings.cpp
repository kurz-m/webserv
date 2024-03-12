#include "Settings.hpp"

const char *NotFoundError::what() const throw() {
  return "Requested Setting or Route not found!";
}

const Setting RouteBlock::find(Token::token_type_t name) const {
  std::vector<Setting>::const_iterator it;
  for(it = settings.begin(); it != settings.end(); ++it) {
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

const RouteBlock *ServerBlock::find(const std::string& uri) const {
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

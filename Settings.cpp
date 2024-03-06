#include "Settings.hpp"

const char *NotFoundError::what() const throw() {
  return "Requested Setting not found!";
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

const Setting HttpBlock::find(Token::token_type_t name) const {
  std::vector<Setting>::const_iterator it;
  for (it = settings.begin(); it != settings.end(); ++it) {
    if (it->name == name) {
      return *it;
    }
  }
  throw NotFoundError();
}

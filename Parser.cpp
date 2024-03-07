#include "Parser.hpp"
#include "HTTP.hpp"
#include "Lexer.hpp"

#include <stdexcept>
#include <sys/stat.h>

Parser::Parser(Lexer &lexer)
    : lexer_(lexer), block_depth_(0), server_count_(0), route_count_(0) {
  next_token_();
  next_token_();
}

Parser::Parser(const Parser &rhs) : lexer_(rhs.lexer_) { *this = rhs; }

Parser &Parser::operator=(const Parser &rhs) {
  if (this != &rhs) {
    current_token_ = rhs.current_token_;
    peek_token_ = rhs.peek_token_;
    block_depth_ = rhs.block_depth_;
    http_ = rhs.http_;
    server_count_ = rhs.server_count_;
    route_count_ = rhs.route_count_;
  }
  return *this;
}

Parser::~Parser() {}

inline void Parser::next_token_() {
  current_token_ = peek_token_;
  peek_token_ = lexer_.next_token();
}

void Parser::parse_config() {
  if (!expect_current_(Token::HTTP)) {
    throw std::invalid_argument("provided config file has no http block");
  }
  if (!expect_peek_(Token::LBRACE)) {
    throw std::invalid_argument("wrong syntax for config file");
  }
  ++block_depth_;
  next_token_();
  next_token_();

  while (!expect_peek_(Token::EF)) {
    if (expect_current_(Token::SERVER)) {
      http_.servers.push_back(parse_serverblock_());
      ++server_count_;
    } else if (current_token_.type & S_BITS) {
      http_.settings.push_back(parse_setting_());
    } else {
      throw std::invalid_argument("wrong syntax for config file");
    }
    next_token_();
  }
  if (expect_current_(Token::RBRACE)) {
    --block_depth_;
  } else {
    throw std::invalid_argument("wrong syntax for config file");
  }
  if (block_depth_ != 0) {
    throw std::invalid_argument("http block is not fully closed");
  }
}

ServerBlock Parser::parse_serverblock_() {
  route_count_ = 0;
  next_token_();
  if (!expect_current_(Token::LBRACE)) {
    throw std::invalid_argument("wrong syntax for config file");
  }
  ++block_depth_;
  next_token_();
  ServerBlock server;
  while (~current_token_.type & (Token::RBRACE | Token::EF)) {
    if (expect_current_(Token::LOCATION)) {
      server.routes.push_back(parse_routeblock_());
    } else if (current_token_.type & S_BITS) {
      server.settings.push_back(parse_setting_());
    } else {
      throw std::invalid_argument("wrong syntax for config file");
    }
    next_token_();
  }
  if (expect_current_(Token::RBRACE)) {
    --block_depth_;
  } else if (expect_current_(Token::EF)) {
    throw std::invalid_argument("missing closing block");
  }
  return server;
}

RouteBlock Parser::parse_routeblock_() {
  if (expect_current_(Token::LOCATION) && expect_peek_(Token::STRING)) {
    check_file_(current_token_.literal);
  }
  next_token_();
  RouteBlock route;
  if (expect_peek_(Token::LBRACE)) {
    ++block_depth_;
    next_token_();
    next_token_();
    while (~current_token_.type & (Token::RBRACE | Token::EF)) {
      route.settings.push_back(parse_setting_());
      next_token_();
    }
  } else {
    throw std::invalid_argument("invalid syntax for config file");
  }
  if (expect_current_(Token::RBRACE)) {
    --block_depth_;
  } else if (expect_current_(Token::EF)) {
    throw std::invalid_argument("missing closing block");
  }
  return route;
}

// TODO: copy all settings from the http block into the server block

Setting Parser::parse_setting_() {
  Setting setting = (Setting){
      .type = Setting::UNSET,
  };
  Token tok = current_token_;
  while (!expect_current_(Token::SEMICOLON)) {
    switch (tok.type) {
    case Token::SERVER_NAME:
    case Token::DEFAULT_TYPE:
    case Token::ROOT:
      setting.type = Setting::STRING;
      setting.name = tok.type;
      setting.str_val = parse_str_value_();
      // implement Token::STRING;
      break;
    case Token::KEEPALIVE_TIMEOUT:
    case Token::CLIENT_MAX_BODY_SIZE:
    case Token::LISTEN:
      setting.type = Setting::INT;
      setting.name = tok.type;
      setting.int_val = parse_int_value_();
      // implement Token::NUMBER;
      break;
    case Token::ALLOW:
    case Token::DENY:
      setting.type = Setting::INT;
      setting.name = tok.type;
      setting.int_val = parse_http_methods_();
      // check for valid methods GET, POST, DEL
      break;
    case Token::AUTOINDEX:
      // implement check for on/off;
      break;
    default:
      // Error because of unknown setting
      break;
    };
    next_token_();
  }
  return setting;
}

inline bool Parser::check_file_(const std::string &file) const {
  struct stat sb;

  if (stat(file.c_str(), &sb) < 0) {
    throw std::invalid_argument("given path/file is not valid");
  }

  switch (sb.st_mode & S_IFMT) {
  case S_IFREG:
    if (expect_current_(Token::LOCATION)) {
      throw std::invalid_argument("file cannot be passed as route option");
      return false;
    }
    return true;
  case S_IFDIR:
    if (current_token_.type & S_BITS) {
      throw std::invalid_argument("got a directory, wanted a file");
      return false;
    }
    return true;
  }
  return false;
}

inline bool Parser::expect_current_(const Token::token_type_t tok) const {
  return current_token_.type == tok;
}

inline bool Parser::expect_peek_(const Token::token_type_t tok) const {
  return peek_token_.type == tok;
}

std::string Parser::parse_str_value_() {
  next_token_();
}

std::string Parser::parse_int_value_() {

}

std::string Parser::parse_http_method_() {

}

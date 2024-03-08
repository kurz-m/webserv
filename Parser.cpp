#include "Parser.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>

#ifndef __verbose__
#define __verbose__
#endif

#define RUN_LOOP 0

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

HttpBlock& Parser::parse_config() {
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
#ifdef __verbose__
  std::cout << "-----Printing out the server settings-----" << std::endl;
  std::vector<ServerBlock>::iterator it;
  for (it = http_.servers.begin(); it != http_.servers.end(); ++it) {
    std::vector<Setting>::iterator sit;
    std::cout << "Server settings:" << std::endl;
    for (sit = it->settings.begin(); sit != it->settings.end(); ++sit) {
      if (sit->type == Setting::STRING) {
        std::cout << Token::reverse_map.at(sit->name) << ": " << sit->str_val
                  << std::endl;
      } else {
        std::cout << Token::reverse_map.at(sit->name) << ": " << sit->int_val
                  << std::endl;
      }
    }
    std::vector<RouteBlock>::iterator rit;
    for (rit = it->routes.begin(); rit != it->routes.end(); ++rit) {
      std::cout << "Route settings: path -> " << rit->path << std::endl;
      std::vector<Setting>::iterator rsit;
      for (rsit = rit->settings.begin(); rsit != rit->settings.end(); ++rsit) {
        if (rsit->type == Setting::STRING) {
          std::cout << Token::reverse_map.at(rsit->name) << ": "
                    << rsit->str_val << std::endl;
        } else {
          std::cout << Token::reverse_map.at(rsit->name) << ": ";
          if (rsit->name & (Token::ALLOW | Token::DENY)) {
            print_method(rsit->int_val);
            std::cout << std::endl;
          } else {
            std::cout << rsit->int_val << std::endl;
          }
        }
      }
    }
  }
#endif
  return http_;
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
  if (http_.settings.size() != 0) {
    std::vector<Setting>::iterator it;
    for (it = http_.settings.begin(); it != http_.settings.end(); ++it) {
      server.settings.push_back(*it);
    }
  }
  while ((current_token_.type & (Token::RBRACE | Token::EF)) == RUN_LOOP) {
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
    check_file_(peek_token_.literal);
  }
  next_token_();
  RouteBlock route;
  route.path = current_token_.literal;
  if (expect_peek_(Token::LBRACE)) {
    ++block_depth_;
    next_token_();
    next_token_();
    while ((current_token_.type & (Token::RBRACE | Token::EF)) == RUN_LOOP) {
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

Setting Parser::parse_setting_() {
  Setting setting = (Setting){
      .type = Setting::UNSET,
      .name = Token::ILLEGAL,
      .str_val = "",
      .int_val = 0,
  };
  Token tok = current_token_;
  next_token_();
  while (!expect_current_(Token::SEMICOLON)) {
    switch (tok.type) {
    case Token::SERVER_NAME:
    case Token::DEFAULT_TYPE:
    case Token::ROOT:
    case Token::INDEX:
      setting.type = Setting::STRING;
      setting.name = tok.type;
      setting.str_val = current_token_.literal;
      break;
    case Token::KEEPALIVE_TIMEOUT:
    case Token::CLIENT_MAX_BODY_SIZE:
    case Token::LISTEN:
      setting.type = Setting::INT;
      setting.name = tok.type;
      setting.int_val = parse_int_value_();
      break;
    case Token::ALLOW:
    case Token::DENY:
      setting.type = Setting::INT;
      setting.name = tok.type;
      setting.int_val |= parse_http_method_();
      break;
    case Token::AUTOINDEX:
      setting.type = Setting::INT;
      setting.name = tok.type;
      setting.int_val = parse_auto_index_();
      break;
    default:
      throw std::invalid_argument("unknown setting provided");
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

inline int Parser::parse_auto_index_() {
  if (expect_current_(Token::TRUE)) {
    return 1;
  } else if (expect_current_(Token::FALSE)) {
    return 0;
  }
  throw std::invalid_argument("invalid option for auto indexing");
}

inline int Parser::parse_int_value_() {
  std::istringstream stream(current_token_.literal);
  int tmp;
  stream >> tmp;
  if (stream.fail()) {
    throw std::invalid_argument("expected a number, got a string");
  }
  return tmp;
}

inline method_e Parser::parse_http_method_() {
  method_e method = method_to_enum(current_token_.literal);
  if (method == UNKNOWN) {
    throw std::invalid_argument("no valid http method provided");
  }
  return method;
}

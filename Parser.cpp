#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Token.hpp"

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

HttpBlock &Parser::parse_config() {
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
    } else if (current_token_.type & Token::HTTP_SETTING) {
      parse_http_settings_(http_);
    } else if (expect_current_(Token::COMMENT)) {
      next_token_();
      continue;
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
#ifdef __verbose__
  // TODO: write a more sophisticated logger class that logs all the settings
#endif
  check_correct_syntax_();
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
  ServerBlock server(http_);
  while ((current_token_.type & (Token::RBRACE | Token::EF)) == RUN_LOOP) {
    if (expect_current_(Token::LOCATION)) {
      server.routes.push_back(parse_routeblock_(server));
    } else if (current_token_.type & Token::SERVER_SETTING) {
      parse_server_settings_(server);
    } else if (expect_current_(Token::COMMENT)) {
      next_token_();
      continue;
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

RouteBlock Parser::parse_routeblock_(const ServerBlock &server) {
  // if (expect_current_(Token::LOCATION) && expect_peek_(Token::STRING)) {
  //   check_file_(peek_token_.literal);
  // }
  next_token_();
  RouteBlock route(server);
  route.path = current_token_.literal;
  if (expect_peek_(Token::LBRACE)) {
    ++block_depth_;
    next_token_();
    next_token_();
    while ((current_token_.type & (Token::RBRACE | Token::EF)) == RUN_LOOP) {
      if (expect_current_(Token::COMMENT)) {
        next_token_();
        continue;
      } else if (current_token_.type & Token::ROUTE_SETTING) {
        parse_route_settings_(route);
        next_token_();
      }
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

void Parser::parse_http_settings_(HttpBlock &http) {
  Token tok = current_token_;
  next_token_();
  while (!expect_current_(Token::SEMICOLON)) {
    switch (tok.type) {
    case Token::CLIENT_MAX_BODY_SIZE:
      http.client_max_body_size = parse_int_value_();
      break;
    case Token::DEFAULT_TYPE:
      http.default_type = current_token_.literal;
      break;
    case Token::KEEPALIVE_TIMEOUT:
      http.keepalive_timeout = parse_int_value_();
      break;
    case Token::ROOT:
      http.root = current_token_.literal;
      break;
    default:
      throw std::invalid_argument("unknown setting for http block provided");
    }
    next_token_();
  }
}

void Parser::parse_server_settings_(ServerBlock &server) {
  Token tok = current_token_;
  next_token_();
  while (!expect_current_(Token::SEMICOLON)) {
    switch (tok.type) {
    case Token::ALLOW:
      server.allow |= parse_http_method_();
      break;
    case Token::DENY:
      server.allow &= ~(parse_http_method_());
      break;
    case Token::AUTOINDEX:
      server.autoindex = parse_auto_index_();
      break;
    case Token::CLIENT_MAX_BODY_SIZE:
      server.client_max_body_size = parse_int_value_();
      break;
    case Token::DEFAULT_TYPE:
      server.default_type = current_token_.literal;
      break;
    case Token::INDEX:
      server.index = current_token_.literal;
      break;
    case Token::KEEPALIVE_TIMEOUT:
      server.keepalive_timeout = parse_int_value_();
      break;
    case Token::LISTEN:
      server.listen = current_token_.literal;
      break;
    case Token::ROOT:
      server.root = current_token_.literal;
      break;
    case Token::SERVER_NAME:
      server.server_name = current_token_.literal;
      break;
    default:
      throw std::invalid_argument("unknown setting for server block provided");
    }
    next_token_();
  }
}

void Parser::parse_route_settings_(RouteBlock &route) {
  Token tok = current_token_;
  if (expect_current_(Token::ALLOW)) {
    route.allow = 0;
  }
  next_token_();
  while (!expect_current_(Token::SEMICOLON)) {
    switch (tok.type) {
    case Token::ALLOW:
      route.allow |= parse_http_method_();
      break;
    case Token::DENY:
      route.allow &= ~(parse_http_method_());
      break;
    case Token::AUTOINDEX:
      route.autoindex = parse_auto_index_();
      break;
    case Token::INDEX:
      route.index = current_token_.literal;
      break;
    default:
      throw std::invalid_argument("unknown setting for route block provided");
    }
    next_token_();
  }
}

inline bool Parser::check_file_(const std::string &file) const {
  struct stat sb;

  if (stat(file.c_str(), &sb) < 0) {
    // FIX: check what to do here together with Flo
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
    // TODO: this can be used for checking the index.html etc files if wanted
    // if (current_token_.type & (Token::STRING_BITS | Token::INT_BITS)) {
    //   throw std::invalid_argument("got a directory, wanted a file");
    //   return false;
    // }
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

inline void Parser::check_correct_syntax_() {
  if (block_depth_ != 0) {
    throw std::invalid_argument("http block is not fully closed");
  }
  std::vector<ServerBlock>::iterator server_it = http_.servers.begin();
  std::vector<RouteBlock>::iterator route_it;
  while (server_it != http_.servers.end()) {
    if (server_it->root.empty() || server_it->listen.empty()) {
#ifdef __verbose__
      // TODO: implement logging of server that got deleted
#endif /** __verbose__ */
      server_it = http_.servers.erase(server_it);
      continue;
    }
    route_it = server_it->routes.begin();
    while (route_it != server_it->routes.end()) {
      std::string full_path = server_it->root + route_it->path;
      if (check_file_(full_path) == false) {
        route_it = server_it->routes.erase(route_it);
        continue;
#ifdef __verbose__
        // TODO: implement logging of route that got deleted
#endif /** __verbose__ */
      }
      ++route_it;
    }
    ++server_it;
  }
}

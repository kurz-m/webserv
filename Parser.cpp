#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>

#include "EventLogger.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Token.hpp"

#define RUN_LOOP 0

Parser::Parser(Lexer &lexer)
    : lexer_(lexer), current_token_(), peek_token_(), block_depth_(0), http_(),
      server_count_(0), route_count_(0), line_count_(1) {
  next_token_();
  next_token_();
}

Parser::Parser(const Parser &rhs)
    : lexer_(rhs.lexer_), current_token_(rhs.current_token_),
      peek_token_(rhs.peek_token_), block_depth_(rhs.block_depth_),
      http_(rhs.http_), server_count_(rhs.server_count_),
      route_count_(rhs.route_count_), line_count_(rhs.line_count_) {}

Parser &Parser::operator=(const Parser &rhs) {
  if (this != &rhs) {
    current_token_ = rhs.current_token_;
    peek_token_ = rhs.peek_token_;
    block_depth_ = rhs.block_depth_;
    http_ = rhs.http_;
    server_count_ = rhs.server_count_;
    route_count_ = rhs.route_count_;
    line_count_ = rhs.line_count_;
  }
  return *this;
}

Parser::~Parser() {}

inline void Parser::next_token_() {
  current_token_ = peek_token_;
  peek_token_ = lexer_.next_token();
}

inline void Parser::check_newline_() {
  if (expect_peek_(Token::NEWLINE)) {
    ++line_count_;
    next_token_();
  }
  next_token_();
}

HttpBlock &Parser::parse_config() {
  if (!expect_current_(Token::HTTP)) {
    print_syntax_error_("Provided config file has no 'http' block");
  }
  if (!expect_peek_(Token::LBRACE)) {
    print_syntax_error_("Wrong syntax. Expected Token::LBRACE");
  }
  ++block_depth_;
  next_token_();
  check_newline_();

  while (!expect_current_(Token::EF)) {
    if (expect_current_(Token::SERVER)) {
      http_.servers.push_back(parse_serverblock_());
      ++server_count_;
    } else if (current_token_.type & Token::HTTP_SETTING) {
      parse_http_settings_(http_);
    } else if (expect_current_(Token::COMMENT)) {
      check_newline_();
    } else if (expect_current_(Token::RBRACE)) {
      --block_depth_;
    } else {
      print_syntax_error_("Wrong syntax. Found no valid setting");
    }
    check_newline_();
  }
  check_correct_syntax_();
  return http_;
}

ServerBlock Parser::parse_serverblock_() {
  route_count_ = 0;
  next_token_();
  if (!expect_current_(Token::LBRACE)) {
    print_syntax_error_("Wrong syntax. Expected Token::LBRACE");
  }
  ++block_depth_;
  check_newline_();
  ServerBlock server(http_);
  while ((current_token_.type & (Token::RBRACE | Token::EF)) == RUN_LOOP) {
    if (expect_current_(Token::LOCATION)) {
      server.routes.push_back(parse_routeblock_(server));
    } else if (current_token_.type & Token::SERVER_SETTING) {
      parse_server_settings_(server);
    } else if (expect_current_(Token::COMMENT)) {
      check_newline_();
      continue;
    } else {
      print_syntax_error_(
          "Wrong syntax. Found no valid setting for server block");
    }
    check_newline_();
  }
  if (expect_current_(Token::RBRACE)) {
    --block_depth_;
  } else if (expect_current_(Token::EF)) {
    print_syntax_error_("Missing closing block. Expected Token::RBRACE");
  }
  return server;
}

RouteBlock Parser::parse_routeblock_(const ServerBlock &server) {
  next_token_();
  RouteBlock route(server);
  route.path = current_token_.literal;
  if (expect_peek_(Token::LBRACE)) {
    ++block_depth_;
    next_token_();
    check_newline_();
    while ((current_token_.type & (Token::RBRACE | Token::EF)) == RUN_LOOP) {
      if (expect_current_(Token::COMMENT)) {
        check_newline_();
        continue;
      } else if (current_token_.type & Token::ROUTE_SETTING) {
        parse_route_settings_(route);
        check_newline_();
      } else {
        print_syntax_error_(
            "Wrong syntax. Found no valid setting for route block");
      }
    }
  } else {
    print_syntax_error_("Wrong syntax. Expected Token::LBRACE");
  }
  if (expect_current_(Token::RBRACE)) {
    --block_depth_;
  } else if (expect_current_(Token::EF)) {
    print_syntax_error_("Missing closing block. Expected Token::RBRACE");
  }
  return route;
}

void Parser::parse_http_settings_(HttpBlock &http) {
  Token tok = current_token_;
  next_token_();
  while (current_token_.type & Token::RUN_PARSING) {
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
      print_syntax_error_(
          "Wrong syntax. Found no valid setting for http block");
    }
    next_token_();
  }
  if (!expect_current_(Token::SEMICOLON)) {
    print_syntax_error_();
  }
}

void Parser::parse_server_settings_(ServerBlock &server) {
  Token tok = current_token_;

  next_token_();
  while (current_token_.type & Token::RUN_PARSING) {
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
      parse_int_value_();
      server.listen = current_token_.literal;
      break;
    case Token::ROOT:
      server.root = current_token_.literal;
      break;
    case Token::SERVER_NAME:
      server.server_name = current_token_.literal;
      break;
    default:
      print_syntax_error_(
          "Wrong syntax. Found no valid setting for server block");
    }
    next_token_();
  }
  if (!expect_current_(Token::SEMICOLON)) {
    print_syntax_error_();
  }
}

void Parser::parse_route_settings_(RouteBlock &route) {
  Token tok = current_token_;
  if (expect_current_(Token::ALLOW)) {
    route.allow = 0;
  }
  next_token_();
  while (current_token_.type & Token::RUN_PARSING) {
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
      print_syntax_error_(
          "Wrong syntax. Found no valid setting for route block");
    }
    next_token_();
  }
  if (!expect_current_(Token::SEMICOLON)) {
    print_syntax_error_("");
  }
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
  print_syntax_error_("Wrong syntax. Found no valid option for autoindex");
  return 0;
}

inline int Parser::parse_int_value_() {
  std::istringstream stream(current_token_.literal);
  int tmp;
  stream >> tmp;
  if (stream.fail()) {
    print_syntax_error_("Expected a Token::NUMBER");
  }
  return tmp;
}

inline method_e Parser::parse_http_method_() {
  method_e method = method_to_enum(current_token_.literal);
  if (method == UNKNOWN) {
    print_syntax_error_("No valid 'http method' provided");
  }
  return method;
}

inline void Parser::check_correct_syntax_() {
  if (block_depth_ != 0) {
    print_syntax_error_("Missing closing block. Expected Token::RBRACE");
  }
  std::vector<ServerBlock>::iterator server_it = http_.servers.begin();
  while (server_it != http_.servers.end()) {
    if (server_it->root.empty() || server_it->listen.empty()) {
      LOG_WARNING("server got erased: " + server_it->server_name);
      server_it = http_.servers.erase(server_it);
      continue;
    }
    ++server_it;
  }
}

inline void Parser::print_syntax_error_(const std::string msg) {
  std::ostringstream oss;
  oss << "Syntax error in line " << line_count_ << ". ";
  if (msg.size() == 0) {
    oss << "Expected Token::SEMICOLON, got Token::"
        << Token::reverse_map.at(current_token_.type) << ".";
  } else {
    oss << msg + ". Have Token::" << Token::reverse_map.at(current_token_.type)
        << ". Literal: " + current_token_.literal + ".";
  }
  throw NotFoundError(oss.str());
}

#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <vector>

#include "Token.hpp"
#include "Settings.hpp"

class Lexer;

class Parser {
public:
  Parser(Lexer &);
  ~Parser();

  HttpBlock parse_config();

private:
  Parser(const Parser &);
  Parser &operator=(const Parser &);

  void next_token_();
  ServerBlock parse_serverblock_();
  RouteBlock parse_routeblock_();
  Setting parse_setting_();
  bool expect_current_(const Token::token_type_t) const;
  bool expect_peek_(const Token::token_type_t) const;

  bool check_file_(const std::string&) const;
  std::string parse_str_value_();
  int parse_int_value_();
  int parse_http_method_();

  Lexer &lexer_;
  Token current_token_;
  Token peek_token_;
  ssize_t block_depth_;
  HttpBlock http_;
  size_t server_count_;
  size_t route_count_;
};

#endif

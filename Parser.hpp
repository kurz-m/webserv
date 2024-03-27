#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <sys/types.h>
#include <vector>

#include "HTTP.hpp"
#include "Settings.hpp"
#include "Token.hpp"

class Lexer;

class Parser {
public:
  Parser(Lexer &);
  ~Parser();

  HttpBlock &parse_config();

private:
  Parser(const Parser &);
  Parser &operator=(const Parser &);

  void next_token_();
  ServerBlock parse_serverblock_();
  RouteBlock parse_routeblock_(const ServerBlock &server);
  bool expect_current_(const Token::token_type_t) const;
  bool expect_peek_(const Token::token_type_t) const;
  void check_newline_();

  void parse_http_settings_(HttpBlock &http);
  void parse_server_settings_(ServerBlock &server);
  void parse_route_settings_(RouteBlock &route);

  int parse_auto_index_();
  int parse_int_value_();
  method_e parse_http_method_();
  void check_correct_syntax_();
  void print_syntax_error_(const std::string msg = "");

  Lexer &lexer_;
  Token current_token_;
  Token peek_token_;
  ssize_t block_depth_;
  HttpBlock http_;
  size_t server_count_;
  size_t route_count_;
  size_t line_count_;
};

#endif /* Parser.hpp */

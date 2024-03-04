#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <vector>

#include "Token.hpp"

class Lexer;

struct Setting {
  enum Type { STRING, INT } type;
  std::string name;
  std::string str_val;
  int inv_val;
};

struct RouteBlock {
  std::string path;
  std::vector<Setting> settings;
};

struct ServerBlock {
  std::vector<Setting> settings;
  std::vector<RouteBlock> routes;
};

struct HttpBlock {
  std::vector<Setting> settings;
  std::vector<ServerBlock> servers;
};

class Parser {
public:
  Parser(Lexer &);
  ~Parser();

  void parse_config();

private:
  Parser(const Parser &);
  Parser &operator=(const Parser &);

  void next_token_();
  ServerBlock parse_serverblock_();
  RouteBlock parse_routeblock_();
  Setting parse_setting_();
  bool expect_current_(const Token::token_type_t) const;
  bool expect_peek_(const Token::token_type_t) const;

  Lexer &lexer_;
  Token current_token_;
  Token peek_token_;
  ssize_t block_depth_;
  HttpBlock http_;
  size_t server_count_;
  size_t route_count_;
};

#endif

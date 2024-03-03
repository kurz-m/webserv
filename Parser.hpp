#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "Token.hpp"

class Lexer;

class Parser {
public:
  Parser(Lexer&);
  ~Parser();

  void parse_config() const;

private:
  Parser(const Parser&);
  Parser& operator=(const Parser&);

  void next_token_();

  Lexer& lexer_;
  Token current_token_;
  Token peek_token_;

};

#endif

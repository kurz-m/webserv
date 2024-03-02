#ifndef __LEXER_HPP__
#define __LEXER_HPP__

#include "Token.hpp"
#include <string>

class Lexer {
public:
  Lexer(const std::string &);
  ~Lexer();

  Token::token_t next_token();

private:
  Lexer(const Lexer &);
  Lexer &operator=(const Lexer &);

  void read_char_();
  char peek_char_();
  Token::token_t new_token_(Token::token_type_t tok_type, char ch);
  void skip_whitespace_();

  std::string input_;
  int position_;
  int read_position_;
  char ch_;
};

#endif //__LEXER_HPP__
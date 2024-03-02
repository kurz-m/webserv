#ifndef __LEXER_HPP__
#define __LEXER_HPP__

#include "Token.hpp"
#include <string>

class Lexer {
public:
  Lexer(const std::string &);
  ~Lexer();

  Token next_token();

private:
  Lexer(const Lexer &);
  Lexer &operator=(const Lexer &);

  void read_char_();
  Token new_token_(const Token::token_type_t tok_type, const char &ch);
  void skip_whitespace_();
  bool is_letter_(const char &ch);
  std::string read_ident_();
  bool is_number_(const char &ch);
  std::string read_number_();

  std::string input_;
  int position_;
  int read_position_;
  char ch_;
};

#endif //__LEXER_HPP__
#include <cctype>
#include <cstddef>

#include "Lexer.hpp"
#include "Token.hpp"

Lexer::Lexer(const std::string &input)
    : input_(input), position_(0), read_position_(0), ch_(0) {
  read_char_();
}

Lexer::Lexer(const Lexer &rhs)
    : input_(rhs.input_), position_(rhs.position_),
      read_position_(rhs.read_position_), ch_(rhs.ch_) {}

Lexer &Lexer::operator=(const Lexer &rhs) {
  if (this != &rhs) {
    input_ = rhs.input_;
    position_ = rhs.position_;
    read_position_ = rhs.read_position_;
    ch_ = rhs.ch_;
  }
  return *this;
}

Lexer::~Lexer() {}

Token Lexer::next_token() {
  Token tok;

  skip_whitespace_();

  switch (ch_) {
  case '{':
    tok = new_token_(Token::LBRACE, ch_);
    break;
  case '}':
    tok = new_token_(Token::RBRACE, ch_);
    break;
  case ';':
    tok = new_token_(Token::SEMICOLON, ch_);
    break;
  case '\n':
    tok = new_token_(Token::NEWLINE, ch_);
    break;
  case '#':
    tok.type = Token::COMMENT;
    tok.literal = read_comment_();
    break;
  case 0:
    tok.type = Token::EF;
    tok.literal = "";
    break;
  default:
    if (is_letter_(ch_)) {
      tok.literal = read_ident_();
      tok.type = Token::lookup_ident(tok.literal);
      return tok;
    } else if (std::isdigit(ch_)) {
      tok = (Token){
          .type = Token::NUMBER,
          .literal = read_number_(),
      };
      return tok;
    } else {
      tok = new_token_(Token::ILLEGAL, ch_);
    }
  }
  read_char_();
  return tok;
}

std::string Lexer::read_comment_() {
  size_t position = position_;
  while (input_[read_position_] != '\n') {
    read_char_();
  }
  return input_.substr(position, read_position_ - position);
}

std::string Lexer::read_ident_() {
  int position = position_;
  while (is_letter_(ch_) || std::isdigit(ch_)) {
    read_char_();
  }
  return input_.substr(position, position_ - position);
}

std::string Lexer::read_number_() {
  int position = position_;
  while (std::isdigit(ch_)) {
    read_char_();
  }
  return input_.substr(position, position_ - position);
}

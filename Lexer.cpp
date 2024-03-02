#include "Lexer.hpp"
#include "Token.hpp"

Lexer::Lexer(const std::string &input)
    : input_(input), position_(0), read_position_(0), ch_(0) {
  read_char_();
}

Lexer::Lexer(const Lexer &rhs) { *this = rhs; }

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

Token::token_t Lexer::next_token() {
  Token::token_t tok;

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
  case '#':
    tok = new_token_(Token::HASH, ch_);
    break;
  }
  read_char_();
  return tok;
}

void Lexer::read_char_() {
  if (read_position_ >= input_.size()) {
    ch_ = 0;
  } else {
    ch_ = input_.at(read_position_);
  }
  position_ = read_position_;
  ++read_position_;
}

char Lexer::peek_char_() {
  if (read_position_ >= input_.size()) {
    return 0;
  } else {
    return input_.at(read_position_);
  }
}

Token::token_t Lexer::new_token_(Token::token_type_t tok_type, char ch) {
  return (Token::token_t){
      .type = tok_type,
      .literal = std::string(1, ch),
  };
}

void Lexer::skip_whitespace_() {
  while (ch_ == 32 || (ch_ > 8 && ch < 14)) {
    read_char_();
  }
}
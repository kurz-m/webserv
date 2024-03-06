#include "Lexer.hpp"
#include "Token.hpp"
#include <cctype>
#include <cstddef>

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
  while (ch_ != '\n') {
    read_char_();
  }
  return input_.substr(position, position_ - position);
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

Token Lexer::new_token_(Token::token_type_t tok_type, const char &ch) {
  return (Token){
      .type = tok_type,
      .literal = std::string(1, ch),
  };
}

void Lexer::skip_whitespace_() {
  while (ch_ == 32 || (ch_ > 8 && ch_ < 14)) {
    read_char_();
  }
}

bool Lexer::is_letter_(const char &ch) {
  return std::isalpha(ch) || ch == '.' || ch == '/' || ch == '_' || ch == '-';
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
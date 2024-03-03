#include "Parser.hpp"
#include "Lexer.hpp"

Parser::Parser(Lexer& lexer) : lexer_(lexer)
{
  lexer_.next_token();
  lexer_.next_token();
}

Parser::~Parser() {}

void Parser::next_token_()
{
  peek_token_ = current_token_;
  current_token_ = lexer_.next_token();
}

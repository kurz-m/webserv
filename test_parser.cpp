#include "Lexer.hpp"
#include "Token.hpp"
#include "Parser.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main() {
  std::ifstream file("./config/default.conf");
  std::string buffer;

  buffer.assign(std::istreambuf_iterator<char>(file),
                std::istreambuf_iterator<char>());

  Lexer lex(buffer);
  Parser parser(lex);
  parser.parse_config();

  // do {
  //   tok_test = lex.next_token();
  //   std::cout << "current tok: " << tok_test.type
  //             << " literal: " << tok_test.literal << std::endl;
  // } while (tok_test != Token::EF);

  file.close();
  return 0;
}

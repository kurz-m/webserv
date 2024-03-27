#include "Lexer.hpp"
#include "Parser.hpp"
#include "Token.hpp"
#include <fstream>
#include <iostream>
#include <string>

// std::string config_string = "http {\n\
//     # default_type                text/html;\n\
//     keepalive_timeout           10;\n\
//     server {\n\
//         listen                  8080;\n\
//         server_name             www.example.com;\n\
//         client_max_body_size    10000;\n\
//         root                    ./data/www;\n\
//         location / {\n\
//             index               index.html;\n\
//             allow               GET POST;\n\
//             deny                DELETE;\n\
//         }\n\
//         location /cgi-bin {\n\
//             autoindex           on;\n\
//             allow               GET\n\
//             deny                POST DELETE;\n\
//         }\n\
//     }\n\
//     server {\n\
//         listen                  3490;\n\
//         server_name             www.example2.com;\n\
//         client_max_body_size    10000;\n\
//         root                    ./data/www;\n\
//         location / {\n\
//             autoindex           on;\n\
//             allow               GET POST;\n\
//             deny                DELETE;\n\
//         }\n\
//     }\n\
// }";

int main() {
  std::ifstream file("./config/default.conf");
  std::string buffer;

  buffer.assign(std::istreambuf_iterator<char>(file),
                std::istreambuf_iterator<char>());

  Lexer lex(buffer);
  Parser parser(lex);
  try {
    parser.parse_config();
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }

  // Token tok_test;
  // do {
  //   tok_test = lex.next_token();
  //   std::cout << "current tok: " << Token::reverse_map.at(tok_test.type)
  //             << " literal: ";
  //   (tok_test.literal == "\n") ? std::cout << "\\n"
  //                              : std::cout << tok_test.literal;
  //   std::cout << std::endl;
  // } while (tok_test != Token::EF);

  file.close();
  return 0;
}

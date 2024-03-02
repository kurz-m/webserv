#ifndef __TOKEN_HPP__
#define __TOKEN_HPP__

#include <map>
#include <string>

class Token {
public:
  enum token_type_t {
    ILLEGAL,
    EF,

    STRING,
    NUMBER,
    // Directives
    HTTP,
    SETTING,
    LOCATION,

    // Keywords
    SERVER_NAME,
    KEEPALIVE_TIMEOUT,
    LISTEN,
    CLIENT_MAX_BODY_SIZE,
    ROOT,
    AUTOINDEX,
    ALLOW,
    DENY,

    // Bool
    TRUE,
    FALSE,
    // Delimiter
    LBRACE,
    RBRACE,
    SEMICOLON,
    HASH
  };

  struct token_t {
    token_type_t type;
    std::string literal;
  };

  static std::map<std::string, token_type_t> create_keywords();
  const static std::map<std::string, token_type_t> keyword_map;

  static std::map<std::string, token_type_t> create_directive();
  const static std::map<std::string, token_type_t> directive_map;

  static token_type_t lookup_ident(std::string &);

  friend class Lexer;
};

#endif //__TOKEN_HPP__
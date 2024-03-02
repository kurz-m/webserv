#ifndef __TOKEN_HPP__
#define __TOKEN_HPP__

#include <map>
#include <string>

struct Token {
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
    DEFAULT_TYPE,
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

  token_type_t type;
  std::string literal;

  bool operator==(const token_type_t &rhs);
  bool operator!=(const token_type_t &rhs);

  static std::map<std::string, token_type_t> create_keywords();
  const static std::map<std::string, token_type_t> keyword_map;

  static token_type_t lookup_ident(std::string &);

  friend class Lexer;
};

#endif //__TOKEN_HPP__
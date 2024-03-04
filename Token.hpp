#ifndef __TOKEN_HPP__
#define __TOKEN_HPP__

#include <map>
#include <string>

#define S_BITS 0x000FFF00

struct Token {
public:
  enum token_type_t {
    ILLEGAL = (1 << 0),
    EF = (1 << 1),

    STRING = (1 << 2),
    NUMBER = (1 << 3),
    // Directives
    HTTP = (1 << 4),
    SERVER = (1 << 5),
    LOCATION = (1 << 6),

    // Keywords
    DEFAULT_TYPE = (1 << 8),
    KEEPALIVE_TIMEOUT = (1 << 9),
    LISTEN = (1 << 10),
    CLIENT_MAX_BODY_SIZE = (1 << 11),
    ROOT = (1 << 12),
    AUTOINDEX = (1 << 13),
    ALLOW = (1 << 14),
    DENY = (1 << 15),
    SERVER_NAME = (1 << 16),

    // Bool
    TRUE = (1 << 20),
    FALSE = (1 << 21),
    // Delimiter
    LBRACE = (1 << 22),
    RBRACE = (1 << 23),
    SEMICOLON = (1 << 24),
    COMMENT = (1 << 25)
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
};

#endif //__TOKEN_HPP__
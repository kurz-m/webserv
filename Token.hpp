#ifndef __TOKEN_HPP__
#define __TOKEN_HPP__

#include <map>
#include <string>

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

    // INT keywords
    AUTOINDEX = (1 << 8),
    ALLOW = (1 << 9),
    DENY = (1 << 10),
    KEEPALIVE_TIMEOUT = (1 << 11),
    CLIENT_MAX_BODY_SIZE = (1 << 12),

    // STRING keywords
    DEFAULT_TYPE = (1 << 16),
    LISTEN = (1 << 17),
    ROOT = (1 << 18),
    SERVER_NAME = (1 << 19),
    INDEX = (1 << 20),

    // Bool
    TRUE = (1 << 24),
    FALSE = (1 << 25),
    // Delimiter
    LBRACE = (1 << 26),
    RBRACE = (1 << 27),
    SEMICOLON = (1 << 28),
    COMMENT = (1 << 29),
    NEWLINE = (1 << 30),

    // Block settings
    HTTP_SETTING = 0x00051800,
    SERVER_SETTING = 0x001F1F00,
    ROUTE_SETTING = 0x00100700,
    RUN_PARSING = 0x0300000C,
  };

  token_type_t type;
  std::string literal;

  bool operator==(const token_type_t &rhs);
  bool operator!=(const token_type_t &rhs);

  static std::map<std::string, token_type_t> create_keywords();
  const static std::map<std::string, token_type_t> keyword_map;

  static std::map<token_type_t, std::string> create_reverse();
  const static std::map<token_type_t, std::string> reverse_map;

  static token_type_t lookup_ident(std::string &);
};

#endif //__TOKEN_HPP__

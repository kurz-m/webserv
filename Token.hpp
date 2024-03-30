#ifndef __TOKEN_HPP
#define __TOKEN_HPP

#include <map>
#include <string>

struct Token {
public:
  enum token_type_t {
    ILLEGAL = (1 << 0), /**< Specifies an invalid token*/
    EF = (1 << 1),      /**< Specifies end of file*/

    STRING = (1 << 2), /**< String value for token*/
    NUMBER = (1 << 3), /**< Number value for token*/

    HTTP = (1 << 4),     /**< Token for http block*/
    SERVER = (1 << 5),   /**< Token for server block*/
    LOCATION = (1 << 6), /**< Token for route block*/

    AUTOINDEX = (1 << 8),             /**< Token for autoindex setting*/
    ALLOW = (1 << 9),                 /**< Token for allowed methods*/
    DENY = (1 << 10),                 /**< Token for denied methods*/
    KEEPALIVE_TIMEOUT = (1 << 11),    /**< Token for client keep alive setting*/
    CLIENT_MAX_BODY_SIZE = (1 << 12), /**< Token for max client body size*/

    DEFAULT_TYPE = (1 << 16), /**< Token for default mime-type*/
    LISTEN = (1 << 17),       /**< Token for listening port setting*/
    ROOT = (1 << 18),         /**< Token for server root folder*/
    SERVER_NAME = (1 << 19),  /**< Token for server name*/
    INDEX = (1 << 20),        /**< Token for the index.html filename*/

    TRUE = (1 << 24),  /**< Token for specifying auto indexing*/
    FALSE = (1 << 25), /**< Token for specifying auto indexing*/

    LBRACE = (1 << 26),    /**< Token for opening a block*/
    RBRACE = (1 << 27),    /**< Token for closing a block*/
    SEMICOLON = (1 << 28), /**< Token for ending a line*/
    COMMENT = (1 << 29),   /**< Token for commenting out a whole line*/
    NEWLINE = (1 << 30),   /**< Token for newline character*/

    HTTP_SETTING = 0x00051800,   /**< Token for possible http settings*/
    SERVER_SETTING = 0x001F1F00, /**< Token for possible server settings*/
    ROUTE_SETTING = 0x00100700,  /**< Token specifying possible route settings*/
    RUN_PARSING = 0x0300000C,    /**< Token for the parsing loop conditiong*/
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

#endif /* Token.hpp */

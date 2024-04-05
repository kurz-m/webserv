#ifndef __TOKEN_HPP
#define __TOKEN_HPP

#include <map>
#include <string>

/**
 * Basic struct for a Token variable.
 *
 * This struct serves as Token during the lexing and parsing of the config file.
 * It holds the different Token types as well as some Tokens to make specific
 * checks during the parsing. Furthermore, 2 lookup tables are implemented to
 * make it more efficient to parse Tokens and also have a predefined set of
 * possible configurations.
 */
struct Token {
public:
  /**
   * Enum for specifying possible Token types.
   *
   * token_type_t is being used in order to specify the possible Token types
   * as well as defining conditions with respect to loops within the Parser or
   * each block of settings.
   */
  enum token_type_t {
    ILLEGAL = (1 << 0), /**< Specifies an invalid token. */
    EF = (1 << 1),      /**< Specifies end of file. */

    STRING = (1 << 2), /**< String value for token. */
    NUMBER = (1 << 3), /**< Number value for token. */

    HTTP = (1 << 4),     /**< Token for http block. */
    SERVER = (1 << 5),   /**< Token for server block. */
    LOCATION = (1 << 6), /**< Token for route block. */

    AUTOINDEX = (1 << 8),          /**< Token for autoindex setting. */
    ALLOW = (1 << 9),              /**< Token for allowed methods. */
    DENY = (1 << 10),              /**< Token for denied methods. */
    KEEPALIVE_TIMEOUT = (1 << 11), /**< Token for client keep alive setting. */
    CLIENT_MAX_BODY_SIZE = (1 << 12), /**< Token for max client body size. */

    DEFAULT_TYPE = (1 << 16), /**< Token for default mime-type. */
    LISTEN = (1 << 17),       /**< Token for listening port setting. */
    ROOT = (1 << 18),         /**< Token for server root folder. */
    SERVER_NAME = (1 << 19),  /**< Token for server name. */
    INDEX = (1 << 20),        /**< Token for the index.html filename. */
    REDIR = (1 << 21),        /**< Token for redirection. */

    TRUE = (1 << 24),  /**< Token for specifying auto indexing. */
    FALSE = (1 << 25), /**< Token for specifying auto indexing. */

    LBRACE = (1 << 26),    /**< Token for opening a block. */
    RBRACE = (1 << 27),    /**< Token for closing a block. */
    SEMICOLON = (1 << 28), /**< Token for ending a line. */
    COMMENT = (1 << 29),   /**< Token for commenting out a whole line. */
    NEWLINE = (1 << 30),   /**< Token for newline character. */

    HTTP_SETTING = 0x00051800,   /**< Token for possible http settings. */
    SERVER_SETTING = 0x001F1F00, /**< Token for possible server settings. */
    ROUTE_SETTING = 0x00300700,  /**< Token for possible route settings. */
    RUN_PARSING = 0x0300000C,    /**< Token for the parsing loop conditiong. */
  };
  /**
   * Public member to hold the Token type.
   *
   * The type member is being used for checking which Token type it has.
   */
  token_type_t type;

  /**
   * Public member which displays the literal representation of the Token.
   */
  std::string literal;

  /**
   * Operator overload to compare Tokens with each other
   *
   * Used to check if 2 Tokens are equal.
   */
  bool operator==(const token_type_t &rhs);

  /**
   * Operator overload to compare Tokens with each other
   *
   * Used to check if 2 Tokens are not equal.
   */
  bool operator!=(const token_type_t &rhs);

  /**
   * Public static member function to create the lookup table for the keyword
   * map.
   *
   * This map is labeled static in order to be accessible for every instance of
   * a Token. Therefore, it is being called within the .cpp file to fill the
   * keyword_map.
   *
   * \return std::map<std::string, token_type_t> keyword map
   */
  const static std::map<std::string, token_type_t> create_keywords();

  /**
   * Lookup table for all specified keywords.
   *
   * This map is being used by the Lexer to compare during Token creation
   * for previously defined Token types to be able to assign the correct type.
   */
  const static std::map<std::string, token_type_t> keyword_map;

  /**
   * Public static member function to create a reverse lookup table for the
   * reverse keyword map.
   *
   * This map is labeled static in order to be accessible for every instance of
   * a Token. Therefore, it is being called within the .cpp file to fill the
   * reverse_map.
   *
   * \return std::map<token_type_t, std::string> reverse map
   */
  const static std::map<token_type_t, std::string> create_reverse();

  /**
   * Reverse lookup table for all specified keywords.
   *
   * This map is being used for emitting errors and finding the correct Token
   * for that. This map is being used for emitting errors and finding the
   * correct Token for that.It makes it possible to print the literal of the
   * erroneous Token.
   * .
   */
  const static std::map<token_type_t, std::string> reverse_map;

  /**
   * Public member function to identify a keyword Token.
   *
   * This function takes a string literal as input argument and uses the
   * keyword_map to find the corresponding Token type. If encountered, it
   * returns the found one, otherwise it returns Token::STRING.
   *
   * \param ident string literal of the current lexical analysis.
   * \return token_type_t Type of the found keyword or STRING.
   *
   */
  static token_type_t lookup_ident(std::string &);
};

#endif /* Token.hpp */

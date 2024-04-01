#ifndef __PARSER_HPP
#define __PARSER_HPP

#include <sys/types.h>
#include <vector>

#include "HTTP.hpp"
#include "Settings.hpp"
#include "Token.hpp"

class Lexer;

/**
 * A class for parsing the webserv settings specified in the config file.
 *
 * The Parser class is used for parsing the webserv config file specified during
 * the program calling or falls back to a default config file. It makes a
 * lexical analysis of the tokens returned by the Lexer.
 */
class Parser {
public:
  /**
   * Parameterized constructor for the Parser class.
   *
   * This constructor is taking a reference to a Lexer instance. Furthermore, it
   * calls next_token_() twice, to establish the starting point for the current
   * token and the peek token.
   *
   * \param input Reference to a Lexer instance.
   */
  Parser(Lexer &);

  /**
   * Destructor of the Parser class.
   */
  ~Parser();

  /**
   * Main public interface to the Parser which returns the parsed config.
   *
   * parse_config() is the main interface other functions can work with when
   * using the Parser. It contains all the logic for creating a HttpBlock
   * struct, which can have multiple server and route blocks.
   *
   * \return The parsed config file within a HttpBlock struct.
   */
  HttpBlock &parse_config();

private:
  Parser(const Parser &);
  Parser &operator=(const Parser &);

  void next_token_();
  ServerBlock parse_serverblock_();
  RouteBlock parse_routeblock_(const ServerBlock &server);
  bool expect_current_(const Token::token_type_t) const;
  bool expect_peek_(const Token::token_type_t) const;
  void check_newline_();

  void parse_http_settings_(HttpBlock &http);
  void parse_server_settings_(ServerBlock &server);
  void parse_route_settings_(RouteBlock &route);

  int parse_auto_index_();
  int parse_int_value_();
  method_e parse_http_method_();
  void check_correct_syntax_();
  void print_syntax_error_(const std::string msg = "");

  Lexer &lexer_;
  Token current_token_;
  Token peek_token_;
  ssize_t block_depth_;
  HttpBlock http_;
  size_t server_count_;
  size_t route_count_;
  size_t line_count_;
};

#endif /* Parser.hpp */

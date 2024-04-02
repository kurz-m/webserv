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
 *
 * \throws NotFoundError when there is a syntax error in the config file.
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
  /**
   * Copy constructor for the Parser class.
   *
   * Constructor takes a const reference to another Parser instance and
   * copies all the elements to a new Parser, thereby making a deep copy.
   *
   * \param rhs Const reference to a Parser instance.
   */
  Parser(const Parser &);

  /**
   * Copy assignment operator of the Lexer class.
   *
   * \param rhs Const reference to a Lexer instance.
   */
  Parser &operator=(const Parser &);

  /**
   * Wrapper function for Lexer::next_token().
   *
   * This private member function calls the Lexer::next_token() function twice
   * in order to fill the current token and also the peek token. It is the main
   * function to move through the tokens created by the Lexer.
   */
  void next_token_();

  /** \name Block Parsing
   * Private member functions for parsing the different blocks.
   */
  /**@{*/
  /**
   * Private member function to parse a ServerBlock.
   *
   * This function is used to parse all possible setting for a server of the
   * webserv. It is possible that this function is called multiple times,
   * because there can be more than 1 server within the http block of the config
   * file.
   *
   * \sa ServerBlock
   *
   * \return Returns the parsed ServerBlock
   */
  ServerBlock parse_serverblock_();

  /**
   * Private member function to parse a RouteBlock.
   *
   * This function is used to parse all possible setting for a route of a
   * specific server. It is possible that this function is called multiple
   * times, because there can be multiple routes within a single server. The
   * parse routeblock function is always called within parsing a ServerBlock.
   *
   * \sa RouteBlock
   *
   * \return Returns the parsed RouteBlock
   */
  RouteBlock parse_routeblock_(const ServerBlock &server);
  /**@}*/

  /** \name Setting Parsing
   * Private member functions for parsing block settings.
   */
  /**@{*/
  /**
   * Private member function to parse a single setting in a http block.
   *
   * This function enables the Parser to parse a single setting that can be
   * specified on the level of an http block.
   *
   * \param http Reference to an HttpBlock.
   */
  void parse_http_settings_(HttpBlock &http);

  /**
   * Private member function to parse a single setting in a server block.
   *
   * This function enables the Parser to parse a single setting that can be
   * specified on the level of an server block.
   *
   * \param server Reference to an ServerBlock.
   */
  void parse_server_settings_(ServerBlock &server);

  /**
   * Private member function to parse a single setting in a route block.
   *
   * This function enables the Parser to parse a single setting that can be
   * specified on the level of an route block.
   *
   * \param route Reference to an RouteBlock.
   */
  void parse_route_settings_(RouteBlock &route);
  /**@}*/

  /** \name Single Option Parsing
   * Private member functions for parsing single block configuration options.
   */
  /**@{*/
  /**
   * Small private member function to parse the values for autoindex.
   *
   * This function checks the value that is being used for the autoindex setting
   * and can throw an error if there is no valid option for autoindex.
   *
   * \return 1 if current token is Token::TRUE, 0 if Token::FALSE.
   */
  int parse_auto_index_();

  /**
   * Small private member function to parse integer values.
   *
   * This function is used to parse integer values from the current token
   * literal value. It is only called on settings that expect integer values.
   * THerefore, it can throw an error, if there was an integer expected but it
   * got something else.
   *
   * \return The parsed integer value.
   */
  int parse_int_value_();

  /**
   * Small private member function to parse the values for the http methods.
   *
   * This function parses the current token literal with method_to_enum() to get
   * the value for the http method setting. It can throw an error when
   * encountering an unknown method.
   *
   * \return The parsed method.
   */
  method_e parse_http_method_();
  /**@}*/

  /** \name Check Token And Syntax
   * Private member functions for checking tokens and syntax.
   */
  /**@{*/
  /**
   * Small private member function to check the current token type.
   *
   * \param tok The expected token type of the current token.
   * \return true if the input is equal the current token, otherwise false.
   */
  bool expect_current_(const Token::token_type_t) const;

  /**
   * Small private member function to check the peek token type.
   *
   * \param tok The expected token type of the peek token.
   * \return true if the input is equal the peek token, otherwise false.
   */
  bool expect_peek_(const Token::token_type_t) const;

  /**
   * Small private member function to check the existence of a newline.
   *
   * This function is used to call next_token_() an additional time if there is
   * a newline. This should enable the Parser to also work when there are no
   * newlines in the cofig file. But this has not been tested yet.
   */
  void check_newline_();

  /**
   * Private member function to check the correct syntax of the config file.
   *
   * This function is called after the parsing is finished to check if all the
   * necessary settings were given. Furthermore, it could delete single servers
   * if they had not set any root or listening setting, because those are
   * mandatory. When erasing a server it uses the LOG_WARNING to write it into
   * the log file which server got deleted.
   */
  void check_correct_syntax_();

  /**
   * Private member function to print out error and throw NotFoundError.
   *
   * This function is called when encountering a syntax error and can be used to
   * see where the syntax error occured. When used without a message, the syntax
   * error is related to a missing Token::SEMICOLON, otherwhise it prints the
   * input message along with the current token to understand whats wrong.
   */
  void print_syntax_error_(const std::string msg = "");
  /**@}*/

  /**
   * Private member attribute of the parser.
   *
   * Reference to an instance of a Lexer. This is being used to have the tokens
   * from the input available.
   */
  Lexer &lexer_;

  /**
   * Private member attribute of the Parser.
   *
   * Variable that holds the current token during parsing.
   * \sa Token
   */
  Token current_token_;

  /**
   * Private member attribute of the parser.
   *
   * Variable that holds the peek token during parsing.
   */
  Token peek_token_;

  /**
   * Private member attribute of the parser.
   *
   * Variable to check the block depth and being able to check if all blocks
   * have been closed properly.
   */
  ssize_t block_depth_;

  /**
   * Private member attribute of the parser.
   *
   * HttpBlock variable that is being returned after parsing the configuration
   * file.
   */
  HttpBlock http_;

  /**
   * Private member attribute of the parser.
   *
   * Variable representing the number of servers within the configuration file.
   * If there is no server present at the end due to wrong configuration or no
   * server got specified it can be used to check for that.
   */
  size_t server_count_;

  /**
   * Private member attribute of the parser.
   *
   * Variable for representing the current line where the Parser is. This is
   * being used for giving the user a better syntax error printing in order to
   * showcase on which line the error occuredl
   */
  size_t line_count_;
};

#endif /* Parser.hpp */

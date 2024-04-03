#ifndef __LEXER_HPP
#define __LEXER_HPP

#include <cstddef>
#include <string>

#include "Token.hpp"

/**
 * A class for creating tokens from the configuration file.
 *
 * The Lexer class is used by the Parser for extracting tokens from the given
 * config file. It only creates tokens without adding context to them.
 */
class Lexer {
public:
  /**
   * Parameterized constructor for the Lexer class.
   *
   * This constructor is taking a const reference to the input, which is the
   * configuration file specified by the User or as a fallback to the default
   * config file. It also calls read_char_() once to start at the first
   * character of the input.
   *
   * \param input Const reference to the input string.
   */
  Lexer(const std::string &);

  /**
   * Destructor of the Lexer class.
   */
  ~Lexer();

  /**
   * Main public interface to the Lexer which returns the next Token.
   *
   * next_token() is the main interface other functions can work with when using
   * the Lexer. It contains all the logic for creating tokens and calls the
   * internal functions from the Lexer class.
   *
   * \return A newly created Token.
   */
  Token next_token();

private:
  /**
   * Copy constructor for the Lexer class.
   *
   * Constructor takes a const reference to another Lexer instance and
   * copies all the elements to a new Lexer, thereby making a deep copy.
   *
   * \param rhs Const reference to a Lexer instance.
   */
  Lexer(const Lexer &);

  /**
   * Copy assignment operator of the Lexer class.
   *
   * \param rhs Const reference to a Lexer instance.
   */
  Lexer &operator=(const Lexer &);

  /**
   * Function for reading the next character of the input.
   */
  __always_inline void read_char_() {
    if (read_position_ >= input_.size()) {
      ch_ = 0;
    } else {
      ch_ = input_.at(read_position_);
    }
    position_ = read_position_;
    ++read_position_;
  }

  __always_inline Token new_token_(const Token::token_type_t tok_type,
                                   const char &ch) {
    return (Token){
        .type = tok_type,
        .literal = std::string(1, ch),
    };
  }

  /**
   * Function to skip spaces within the input.
   *
   * This private member function runs as long as it encounters spaces. Spaces
   * are specified by the different tab values and whitespaces.
   */
  __always_inline void skip_whitespace_() {
    while (ch_ == 32 || (ch_ > 8 && ch_ < 10) || (ch_ > 10 && ch_ < 14)) {
      read_char_();
    }
  }

  /**
   * Function to check if the input parameter is a letter.
   *
   * This function is designed to check if the input parameter is either an
   * alphabetical value or '.', '/', '_', '-', respectively.
   *
   * \param ch Const reference to a character.
   * \\return true or false.
   */
  __always_inline bool is_letter_(const char &ch) {
    return std::isalpha(ch) || ch == '.' || ch == '/' || ch == '_' || ch == '-';
  }

  /**
   * Function for reading a full keyword.
   *
   * This function is called within the default case of next_token() to read a
   * full identifier from the config file which can be then checked if it is a
   * keyword within the look up table for keywords from Token.
   *
   * \return The string literal read from the config file.
   */
  std::string read_ident_();

  /**
   * Function for reading a number.
   *
   * This function is called within the default case of next_token() if the
   * first char is a digit to read a number from the config file.
   *
   * \return The number as string literal read from the config file.
   */
  std::string read_number_();

  /**
   * Function for reading a comment.
   *
   * This function is called when the char is a '#'. It then continues until it
   * encounters a newline '\n'. This is useful for commenting out whole lines by
   * adding '#' to the front of the line.
   *
   * \return The line as string literal read from the config file.
   */
  std::string read_comment_();

  /**
   * A private member attribute holding the input from the config file.
   */
  std::string input_;

  /**
   * A private member attribute holdingthe current position on the input string.
   */
  size_t position_;

  /**
   * One position ahead of the current position on the input string.
   *
   * This private member attribute serves as peek to the next character and
   * decide what to do based on it.
   */
  size_t read_position_;

  /**
   * A private member attribute for the Lexer.
   *
   * When read_char_() is being called, ch_ then gets the character value read
   * from the read position of the input string 'input_.at(read_position_)'.
   */
  char ch_;
};

#endif /* Lexer.hpp */

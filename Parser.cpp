#include "Parser.hpp"
#include "Lexer.hpp"
#include <stdexcept>

Parser::Parser(Lexer& lexer) : lexer_(lexer), block_depth_(0)
{
  lexer_.next_token();
  lexer_.next_token();
}

Parser::~Parser() {}

void Parser::next_token_()
{
  peek_token_ = current_token_;
  current_token_ = lexer_.next_token();
}

void Parser::parse_config()
{
  if (!expect_current_(Token::HTTP)) {
    throw std::invalid_argument("provided config file has no http block");
  }
  if (!expect_peek_(Token::LBRACE)) {
    throw std::invalid_argument("wrong syntax for config file");
  }
  ++block_depth_;
  next_token_();
  next_token_();

  while (!expect_current_(Token::EF)) {
    if (expect_current_(Token::SERVER)) {
      http_.servers.push_back(parse_serverblock_());
    }
    else if (current_token_.type & S_BITS) {
      http_.settings.push_back(parse_setting_());
    }
    else {
      throw std::invalid_argument("wrong syntax for config file");
    }
  }

}

ServerBlock Parser::parse_serverblock_()
{
  route_count_ = 0;
  next_token_();
  if (!expect_current_(Token::LBRACE)) {
    throw std::invalid_argument("wrong syntax for config file");
  }
  ++block_depth_;
  next_token_();
  while (~current_token_.type & (Token::RBRACE | Token::EF)) {
    if (expect_current_(Token::LOCATION)) {
      http_.servers[server_count_].routes.push_back(parse_routeblock_());
    }
    else if (current_token_.type & S_BITS) {
      http_.servers[server_count_].settings.push_back(parse_setting_());
    } else {
      throw std::invalid_argument("wrong syntax for config file");
    }
  }
  if (expect_current_(Token::RBRACE)) {
    --block_depth_;
  } else if (expect_current_(Token::EF)) {
    throw std::invalid_argument("missing closing block");
  }
  ++server_count_;
}

RouteBlock Parser::parse_routeblock_()
{
  next_token_();
  if (expect_current_(Token::STRING) && expect_peek_(Token::LBRACE)) {
    ++block_depth_;
    // implement path check here and everything else here
    next_token_();
    next_token_();
    while (~current_token_.type & (Token::RBRACE | Token::EF)) {
      http_.servers[server_count_].routes[route_count_].settings.push_back(parse_setting_());
    }
  }
  if (expect_current_(Token::RBRACE)) {
    --block_depth_;
  } else if (expect_current_(Token::EF)) {
    throw std::invalid_argument("missing closing block");
  }
  ++route_count_;
}

Setting Parser::parse_setting_()
{
  switch (current_token_.type) {
    case Token::SERVER_NAME:
    case Token::DEFAULT_TYPE:
    case Token::ROOT:
      // implement Token::STRING;
      break;
    case Token::KEEPALIVE_TIMEOUT:
    case Token::CLIENT_MAX_BODY_SIZE:
    case Token::LISTEN:
      // implement Token::NUMBER;
      break;
    case Token::ALLOW:
    case Token::DENY:
      // check for valid methods GET, POST, DEL
      break;
    case Token::AUTOINDEX:
      // implement check for on/off;
      break;
    default:
      // Error because of unknown setting
      break;
  };
}

bool Parser::expect_current_(const Token::token_type_t tok) const
{
  return current_token_.type == tok;
}

bool Parser::expect_peek_(const Token::token_type_t tok) const
{
  return peek_token_.type == tok;
}
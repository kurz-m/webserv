#include "Token.hpp"

#include <stdexcept>

std::map<std::string, Token::token_type_t> Token::create_keywords() {
  std::map<std::string, token_type_t> loc_map;
  loc_map.insert(std::make_pair("http", HTTP));
  loc_map.insert(std::make_pair("server", SERVER));
  loc_map.insert(std::make_pair("location", LOCATION));
  loc_map.insert(std::make_pair("server_name", SERVER_NAME));
  loc_map.insert(std::make_pair("default_type", DEFAULT_TYPE));
  loc_map.insert(std::make_pair("keepalive_timeout", KEEPALIVE_TIMEOUT));
  loc_map.insert(std::make_pair("listen", LISTEN));
  loc_map.insert(std::make_pair("client_max_body_size", CLIENT_MAX_BODY_SIZE));
  loc_map.insert(std::make_pair("root", ROOT));
  loc_map.insert(std::make_pair("autoindex", AUTOINDEX));
  loc_map.insert(std::make_pair("allow", ALLOW));
  loc_map.insert(std::make_pair("deny", DENY));
  loc_map.insert(std::make_pair("on", TRUE));
  loc_map.insert(std::make_pair("off", FALSE));
  loc_map.insert(std::make_pair("index", INDEX));
  return loc_map;
}

const std::map<std::string, Token::token_type_t> Token::keyword_map =
    create_keywords();

std::map<Token::token_type_t, std::string> Token::create_reverse() {
  std::map<Token::token_type_t, std::string> loc_map;
  loc_map.insert(std::make_pair(ILLEGAL, "ILLEGAL"));
  loc_map.insert(std::make_pair(EF, "End Of File"));
  loc_map.insert(std::make_pair(STRING, "STRING"));
  loc_map.insert(std::make_pair(NUMBER, "NUMBER"));
  loc_map.insert(std::make_pair(HTTP, "HTTP"));
  loc_map.insert(std::make_pair(SERVER, "SERVER"));
  loc_map.insert(std::make_pair(LOCATION, "LOCATION"));
  loc_map.insert(std::make_pair(SERVER_NAME, "SERVER_NAME"));
  loc_map.insert(std::make_pair(DEFAULT_TYPE, "DEFAULT_TYPE"));
  loc_map.insert(std::make_pair(KEEPALIVE_TIMEOUT, "KEEPALIVE_TIMEOUT"));
  loc_map.insert(std::make_pair(LISTEN, "LISTEN"));
  loc_map.insert(std::make_pair(CLIENT_MAX_BODY_SIZE, "CLIENT_MAX_BODY_SIZE"));
  loc_map.insert(std::make_pair(ROOT, "ROOT"));
  loc_map.insert(std::make_pair(AUTOINDEX, "AUTOINDEX"));
  loc_map.insert(std::make_pair(ALLOW, "ALLOW"));
  loc_map.insert(std::make_pair(DENY, "DENY"));
  loc_map.insert(std::make_pair(TRUE, "ON"));
  loc_map.insert(std::make_pair(FALSE, "OFF"));
  loc_map.insert(std::make_pair(INDEX, "INDEX"));
  loc_map.insert(std::make_pair(LBRACE, "LBRACE"));
  loc_map.insert(std::make_pair(RBRACE, "RBRACE"));
  loc_map.insert(std::make_pair(SEMICOLON, "SEMICOLON"));
  loc_map.insert(std::make_pair(COMMENT, "COMMENT"));
  loc_map.insert(std::make_pair(NEWLINE, "NEWLINE"));
  return loc_map;
}

const std::map<Token::token_type_t, std::string> Token::reverse_map =
    create_reverse();

Token::token_type_t Token::lookup_ident(std::string &ident) {
  try {
    token_type_t tok = keyword_map.at(ident);
    return tok;
  } catch (std::out_of_range &e) {
  }
  return STRING;
}

bool Token::operator==(const token_type_t &rhs) { return type == rhs; }

bool Token::operator!=(const token_type_t &rhs) { return type != rhs; }

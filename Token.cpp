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
  loc_map.insert(std::make_pair("idnex", INDEX));
  return loc_map;
}

const std::map<std::string, Token::token_type_t> Token::keyword_map =
    create_keywords();

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
#include "HTTPResponse.hpp"
#include "Settings.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

static const std::string proto_ = "HTTP/1.1";

HTTPResponse::HTTPResponse(const ServerBlock &config) : HTTPBase(config) {}

HTTPResponse::~HTTPResponse() {}

HTTPResponse::HTTPResponse(const HTTPResponse &cpy) : HTTPBase(cpy) {}

HTTPResponse &HTTPResponse::operator=(const HTTPResponse &other) {
  HTTPBase::operator=(other);
  return *this;
}

void HTTPResponse::resolve_uri_() {
  // loop all route blocks and check if the URI is part of them.
  // have some sort of route hierarchy, e.g. /pictures/ has a higher hierarchy
  // than '/'. a URI of /pictures/xy.jpg is part of both we need to find the one
  // with the highest hierarchy
  try {
    if (parsed_header_.at("URI") == "/") {
      const RouteBlock& route = config_.find("/");

      parsed_header_.at("URI") = config_.find(Token::ROOT).str_val + "/" +
                                 route.find(Token::INDEX).str_val;
    } else {
      parsed_header_.at("URI") =
          config_.find(Token::ROOT).str_val + parsed_header_.at("URI");
    }
  } catch (std::exception &e) {
  }
}

void HTTPResponse::make_header_() {
  std::ifstream file(parsed_header_.at("URI").c_str());
  std::ostringstream oss;
  if (file.is_open()) {
    status_code_ = 200;
  } else {
    status_code_ = 404;
    oss << "./status-pages/" << status_code_ << ".html";
    file.open(oss.str().c_str());
    oss.str("");
  }
  body_.assign(std::istreambuf_iterator<char>(file),
               std::istreambuf_iterator<char>());
  oss << proto_ << " " << status_code_ << " " << status_map_.at(status_code_)
      << "\r\n";
  oss << "Content-Type: text/html\r\n";
  oss << "Content-Length: " << body_.length() << "\r\n";
  oss << "\r\n";
  buffer_ = oss.str();
  buffer_ += body_;
  file.close();
}

void HTTPResponse::prepare_for_send() {
  resolve_uri_();
  make_header_();
#ifdef __verbose__
  std::cout << "buffer: " << buffer_ << std::endl;
#endif
}

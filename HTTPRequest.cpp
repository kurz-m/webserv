#include "HTTPRequest.hpp"
#include <sstream>
#include <string>
#include <iostream>

HTTPRequest::HTTPRequest() {}

HTTPRequest::~HTTPRequest() {}

HTTPRequest::HTTPRequest(const HTTPRequest &cpy) : HTTPBase(cpy) {}

HTTPRequest &HTTPRequest::operator=(const HTTPRequest &other) {
  HTTPBase::operator=(other);
  return *this;
}

void HTTPRequest::parse_header() {
  body_ = buffer_.substr(buffer_.find("\r\n\r\n") + 4);
  buffer_ = buffer_.substr(0, buffer_.find("\r\n\r\n"));
  std::istringstream iss(buffer_);
  std::string line;
  std::getline(iss, line);
  std::istringstream new_iss(line);
  line.erase(line.find("\r"));
  std::string method;
  new_iss >> method;
  if (method == "GET") {
    method_ = GET;
  }
  else if ( method == "POST") {
    method_ = POST;
  }
  else if (method == "DELETE") {
    method_ = DELETE;
  }
  else {
    method_ = UNKNOWN;
  }
  new_iss >> parsed_header_["URI"] >> parsed_header_["PROTOCOL"];
  while (std::getline(iss, line)) {
    size_t pos = line.find(":");
    if (pos == std::string::npos) {
      continue;
    }
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    parsed_header_.insert(std::make_pair(key, value));
  }
  if (parsed_header_.find("Content-Length") == parsed_header_.end()) {
    parsed_header_.insert(std::make_pair("Content-Length", "0"));
  }
  std::map<std::string, std::string>::iterator it;
  for (it = parsed_header_.begin(); it != parsed_header_.end(); ++it) {
    std::cout << it->first << ": " << it->second << std::endl;
  }
}

void HTTPRequest::parse_body() {}
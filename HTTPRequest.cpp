#include "HTTPRequest.hpp"
#include "Socket.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

HTTPRequest::HTTPRequest(const ServerBlock &config)
    : HTTPBase(config), parsed_header_(), method_(UNKNOWN) {}

HTTPRequest::~HTTPRequest() {}

HTTPRequest::HTTPRequest(const HTTPRequest &cpy)
    : HTTPBase(cpy), parsed_header_(), method_(UNKNOWN) {}

HTTPRequest &HTTPRequest::operator=(const HTTPRequest &other) {
  HTTPBase::operator=(other);
  return *this;
}

Socket::status HTTPRequest::parse_header() {
  body_ = buffer_.substr(buffer_.find("\r\n\r\n") + 4);
  buffer_ = buffer_.substr(0, buffer_.find("\r\n\r\n"));
  std::istringstream iss(buffer_);
  std::string line;
  std::getline(iss, line);
  std::istringstream new_iss(line);
  line.erase(line.find("\r"));
  std::string method;
  new_iss >> method;
  method_ = method_to_enum(method);
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
  } else {
    size_t cont_len = std::atoi(parsed_header_.at("Content-Length").c_str());
    if (cont_len > body_.length()) {
      // URECV
#ifdef __verbose__
      std::cout << __LINE__ << "Client did not send the complete content yet"
                << std::endl;
#endif
      return Socket::URECV;
    }
  }
#ifdef __verbose__
  std::map<std::string, std::string>::iterator it;
  for (it = parsed_header_.begin(); it != parsed_header_.end(); ++it) {
    std::cout << it->first << ": " << it->second << std::endl;
  }
#endif
  return Socket::READY;
}

void HTTPRequest::parse_body() {}

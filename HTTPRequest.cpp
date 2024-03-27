#include "HTTPRequest.hpp"
#include "EventLogger.hpp"
#include "Socket.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

HTTPRequest::HTTPRequest(const ServerBlock &config)
    : HTTPBase(config), parsed_header_(), method_(UNKNOWN) {}

HTTPRequest::~HTTPRequest() {}

HTTPRequest::HTTPRequest(const HTTPRequest &cpy)
    : HTTPBase(cpy), parsed_header_(), method_(UNKNOWN) {
  *this = cpy;
}

HTTPRequest &HTTPRequest::operator=(const HTTPRequest &other) {
  HTTPBase::operator=(other);
  if (this != &other) {
    parsed_header_ = other.parsed_header_;
    method_ = other.method_;
  }
  return *this;
}

ISocket::status HTTPRequest::parse_header() {
  size_t end_of_header_pos = buffer_.find("\r\n\r\n");
  if (end_of_header_pos == std::string::npos) {
    LOG_DEBUG("Client did not send the complete content yet");
    return ISocket::URECV;
  }
  body_ = buffer_.substr(end_of_header_pos + 4);
  buffer_ = buffer_.substr(0, end_of_header_pos);
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
  }
  size_t cont_len = std::atoi(parsed_header_.at("Content-Length").c_str());
  if ((cont_len > 0 && cont_len > config_.client_max_body_size) ||
      (cont_len == 0 && body_.length() >= config_.client_max_body_size)) {
    status_code_ = 413;
  }
  if (cont_len > 0 && cont_len > body_.length()) {
    // URECV
    LOG_DEBUG("Client did not send the complete content yet");
    return ISocket::URECV;
  }
  std::map<std::string, std::string>::iterator it;
  for (it = parsed_header_.begin(); it != parsed_header_.end(); ++it) {
    LOG_DEBUG(it->first + ": " + it->second);
  }

  return ISocket::PREPARE_SEND;
}

void HTTPRequest::parse_body() {}

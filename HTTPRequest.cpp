#include "HTTPRequest.hpp"
#include "EventLogger.hpp"
#include "Socket.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

HTTPRequest::HTTPRequest(const ServerBlock &config)
    : HTTPBase(config), parsed_header_(), method_(UNKNOWN), tbr_(0),
      header_parsed_(false), keep_alive_(true) {}

HTTPRequest::~HTTPRequest() {}

HTTPRequest::HTTPRequest(const HTTPRequest &cpy)
    : HTTPBase(cpy), parsed_header_(cpy.parsed_header_), method_(cpy.method_),
      tbr_(cpy.tbr_), header_parsed_(cpy.header_parsed_),
      keep_alive_(cpy.keep_alive_) {}

HTTPRequest &HTTPRequest::operator=(const HTTPRequest &other) {
  HTTPBase::operator=(other);
  if (this != &other) {
    parsed_header_ = other.parsed_header_;
    method_ = other.method_;
    tbr_ = other.tbr_;
    header_parsed_ = other.header_parsed_;
    keep_alive_ = other.keep_alive_;
  }
  return *this;
}

ISocket::status HTTPRequest::parse_header() {
  if (!header_parsed_) {
    size_t end_of_header_pos = buffer_.find("\r\n\r\n");
    if (end_of_header_pos == std::string::npos) {
      LOG_DEBUG("server: " + config_.server_name + ": header incomplete");
      return ISocket::URECV;
    }
    header_ = buffer_.substr(0, end_of_header_pos + 4);
    std::istringstream iss(header_);
    std::string line;
    std::getline(iss, line);
    std::istringstream new_iss(line);
    line.erase(std::find(line.begin(), line.end(), '\r'));
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
      value.erase(std::find(value.begin(), value.end(), ' '));
      value.erase(std::find(value.begin(), value.end(), '\r'));
      parsed_header_.insert(std::make_pair(key, value));
    }
    if (parsed_header_.find("Content-Length") == parsed_header_.end()) {
      parsed_header_.insert(std::make_pair("Content-Length", "0"));
    }
    if (parsed_header_.find("Connection") == parsed_header_.end()) {
      parsed_header_.insert(std::make_pair("Connection", "keep-alive"));
    }
    header_parsed_ = true;
  }
  if (parsed_header_.at("Connection") == "close") {
    keep_alive_ = false;
  }
  size_t cont_len = std::atoi(parsed_header_.at("Content-Length").c_str());
  if (cont_len > 0 && cont_len > (tbr_ - header_.length())) {
    LOG_DEBUG("server: " + config_.server_name + ": body incomplete");
    return ISocket::URECV;
  }
  parse_body();
  std::map<std::string, std::string>::iterator it;
  for (it = parsed_header_.begin(); it != parsed_header_.end(); ++it) {
    LOG_DEBUG(it->first + ": " + it->second);
  }

  return ISocket::PREPARE_SEND;
}

void HTTPRequest::parse_body() {
  LOG_DEBUG("parse body")
  body_ = buffer_.substr(header_.size());
}

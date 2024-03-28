#include "HTTPBase.hpp"
#include <sstream>

std::map<int, std::string> HTTPBase::create_map_() {
  std::map<int, std::string> map;
  map[200] = "OK";
  map[201] = "Created";
  map[204] = "No Content";
  map[303] = "See Other";
  map[400] = "Bad Request";
  map[401] = "Unauthorized";
  map[403] = "Forbidden";
  map[404] = "Not Found";
  map[413] = "Request Entity Too Large";
  map[500] = "Internal Server Error";
  map[501] = "Not Implemented";
  return map;
}

const std::map<int, std::string> HTTPBase::status_map_ =
    HTTPBase::create_map_();

HTTPBase::HTTPBase(const ServerBlock &config)
    : buffer_(), body_(), header_(), status_code_(), config_(config) {}

HTTPBase::HTTPBase(const HTTPBase &cpy)
    : buffer_(), body_(), header_(), status_code_(), config_(cpy.config_) {
  *this = cpy;
}

HTTPBase &HTTPBase::operator=(const HTTPBase &other) {
  if (this != &other) {
    buffer_ = other.buffer_;
    body_ = other.body_;
    header_ = other.header_;
    status_code_ = other.status_code_;
  }
  return *this;
}

HTTPBase::~HTTPBase() {}

void HTTPBase::parse_buffer_() {}

void HTTPBase::reset() {
  buffer_.clear();
  body_.clear();
  header_.clear();
  status_code_ = 0;
}

std::string
HTTPBase::create_status_html(int status_code,
                             const std::string &message /* = "" */) const {
  std::ostringstream oss;

  oss << status_code << " " << status_map_.at(status_code);
  return "<html>"
         "<head><title>" +
         oss.str() +
         "</title></head>"
         "<body>"
         "<center><h1>" +
         oss.str() +
         "</h1><div>" + message + "</div></center>"
         "<hr><center>webserv by makurz and flauer</center>"
         "</body>"
         "</html>";
}

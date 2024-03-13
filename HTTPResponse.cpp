#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
// #include <unistd.h>

#include "CGI.hpp"
#include "HTTPResponse.hpp"
#include "Settings.hpp"

static const std::string proto_ = "HTTP/1.1";

HTTPResponse::HTTPResponse(const ServerBlock &config) : HTTPBase(config) {
  root_ = config_.find(Token::ROOT).str_val;
}

HTTPResponse::~HTTPResponse() {}

HTTPResponse::HTTPResponse(const HTTPResponse &cpy) : HTTPBase(cpy) {}

HTTPResponse &HTTPResponse::operator=(const HTTPResponse &other) {
  HTTPBase::operator=(other);
  return *this;
}

enum uri_state {
  DIRECTORY = (1 << 0),
  FIL = (1 << 1),
  FAIL = (1 << 2),
  AUTO = (1 << 3),
  INDEX = (1 << 4),
  CGI = (1 << 5),
};

template <typename T>
uint8_t HTTPResponse::check_list_dir_(const T &curr_conf, HTTPRequest &req) {
  try {
    std::string g_index = curr_conf.find(Token::INDEX).str_val;
    g_index = root_ + "/" + g_index;
    req.parsed_header_.at("URI") = g_index;
    return (DIRECTORY | INDEX);
  } catch (NotFoundError &e) {
    try {
      int auto_ = curr_conf.find(Token::AUTOINDEX).int_val;
      if (auto_) {
        return (DIRECTORY | AUTO);
      } else {
        return (DIRECTORY);
      }
    } catch (NotFoundError &e) {
      return (DIRECTORY);
    }
  }
}

bool ends_with(const std::string &str, const std::string &extension) {
  if (extension.size() > str.size())
    return false;
  return std::equal(extension.rbegin(), extension.rend(), str.rbegin());
}

// bool check_for_cgi_(const std::string &uri, )

uint8_t HTTPResponse::check_uri_(HTTPRequest &req) {
  struct stat sb;
  std::string uri_ = req.parsed_header_.at("URI");
  const RouteBlock *route = config_.find(uri_);

  // TODO: make better function to check for CGI
  if (uri_.find("/cgi-bin") != std::string::npos) {
    return CGI;
  }
  uri_ =root_ + "/" + uri_;
  if (stat(uri_.c_str(), &sb) < 0) {
    return FAIL;
  }
  switch (sb.st_mode & S_IFMT) {
  case S_IFREG:
    return FIL;
  case S_IFDIR:
    if (route) {
      return check_list_dir_(*route, req);
    } else {
      return check_list_dir_(config_, req);
    }
  }
  return FAIL;
}

void HTTPResponse::call_cgi_(HTTPRequest &req) {
  body_ += CGI::call_cgi(req.parsed_header_.at("URI"), req);
  // std::cout << "cgi call returned!" << std::endl;
  // std::cout << body_ << std::endl;
  status_code_ = 200;
}

void HTTPResponse::read_file_(std::ifstream &file) {
  body_.assign(std::istreambuf_iterator<char>(file),
               std::istreambuf_iterator<char>());
}

void HTTPResponse::make_header_() {
  std::ostringstream oss;
  oss << proto_ << " " << status_code_ << " " << status_map_.at(status_code_)
      << "\r\n";
  oss << "Content-Type: text/html\r\n";
  oss << "Content-Length: " << body_.length() << "\r\n";
  oss << "\r\n";
  buffer_ = oss.str();
  buffer_ += body_;
}

void HTTPResponse::prepare_for_send(HTTPRequest &req) {
  uint8_t mask = check_uri_(req);
  std::ifstream file;
  switch (mask) {
  case CGI:
    call_cgi_(req);
    break;
  case (DIRECTORY | AUTO): // -> list dir
    // TODO: create function for index_of
    status_code_ = 200;
    return;
  case (DIRECTORY): // -> 403
    // Return 403
    status_code_ = 403;
    body_.assign(create_status_html(status_code_));
    break;
  case (DIRECTORY | INDEX): // -> index.html file
  case (FIL):               // -> send file
    // check if file exists
    file.open(req.parsed_header_.at("URI").c_str());
    if (file.is_open()) {
      status_code_ = 200;
      read_file_(file);
    } else {
      status_code_ = 404;
      body_.assign(create_status_html(status_code_));
    }
    break;
  case (FAIL): // -> 404
    // 404
    status_code_ = 404;
    body_.assign(create_status_html(status_code_));
    break;
  default:
    std::cerr << "not intended, catch me" << std::endl;
    return;
  }
  make_header_();
#ifdef __verbose__
  std::cout << "buffer: " << buffer_ << std::endl;
#endif
}

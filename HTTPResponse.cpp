#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

#include "HTTPResponse.hpp"
#include "Settings.hpp"

static const std::string proto_ = "HTTP/1.1";

HTTPResponse::HTTPResponse(const ServerBlock &config,
                           const HTTPRequest &request)
    : HTTPBase(config), request_(request) {}

HTTPResponse::~HTTPResponse() {}

HTTPResponse::HTTPResponse(const HTTPResponse &cpy)
    : HTTPBase(cpy), request_(cpy.request_) {}

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
uint8_t HTTPResponse::check_list_dir_(const T &curr_conf) {
  try {
    std::string g_index = curr_conf.find(Token::INDEX).str_val;
    // parsed_header_.at("URI") =
    curr_conf.find(Token::ROOT).str_val + "/" + g_index;
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

static inline bool ends_with(const std::string &str,
                             const std::string &extension) {
  if (extension.size() > str.size())
    return false;
  return std::equal(extension.rbegin(), extension.rend(), str.rbegin());
}

uint8_t HTTPResponse::check_uri_(const std::string &uri) {
  struct stat sb;
  std::string uri_ = request_.parsed_header_.at("URI");
  const RouteBlock *route = config_.find(uri_);

  if (ends_with(uri_, ".py")) {
    return CGI;
  }

  if (route) {
    uri_ = route->find(Token::ROOT).str_val + "/" + uri;
  } else {
    uri_ = config_.find(Token::ROOT).str_val + "/" + uri;
  }
  if (stat(uri_.c_str(), &sb) < 0) {
    return FAIL;
  }
  switch (sb.st_mode & S_IFMT) {
  case S_IFREG:
    return FIL;
  case S_IFDIR:
    if (route) {
      return check_list_dir_(*route);
    } else {
      return check_list_dir_(config_);
    }
  }
  return FAIL;
}

std::string HTTPResponse::call_cgi_() {}

void HTTPResponse::make_header_(std::ifstream &file) {

  switch (status_code_) {
  case (200):
    // create response with the whatever
    body_.assign(std::istreambuf_iterator<char>(file),
                 std::istreambuf_iterator<char>());
    break;
  case (404):
  case (403):
    // buffer = status_to_string(status_code_);
    body_.assign(create_status_html(status_code_));
  }

  std::ostringstream oss;
  oss << proto_ << " " << status_code_ << " " << status_map_.at(status_code_)
      << "\r\n";
  oss << "Content-Type: text/html\r\n";
  oss << "Content-Length: " << body_.length() << "\r\n";
  oss << "\r\n";
  buffer_ = oss.str();
  buffer_ += body_;
}

void HTTPResponse::prepare_for_send() {
  uint8_t mask = check_uri_(request_.parsed_header_.at("URI"));
  std::ifstream file;
  switch (mask) {
  case CGI:

  case (DIRECTORY | AUTO): // -> list dir
    // TODO: create function for index_of
    body_ = create_list_dir_();
    status_code_ = 200;
    return;
  case (DIRECTORY): // -> 403
    // Return 403
    status_code_ = 403;
    break;
  case (DIRECTORY | INDEX): // -> index.html file
  case (FIL):               // -> send file
    // check if file exists
    file.open(request_.parsed_header_.at("URI").c_str());
    if (file.is_open()) {
      status_code_ = 200;
    } else {
      status_code_ = 404;
    }
    break;
  case (FAIL): // -> 404
    // 404
    status_code_ = 404;
    break;
  default:
    std::cerr << "not intended, catch me" << std::endl;
    return;
  }
  make_header_(file);
#ifdef __verbose__
  std::cout << "buffer: " << buffer_ << std::endl;
#endif
}

const std::string list_dir_head =
    "<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.5\">"
    "<title>Directory Listing</title> <style> body { font-family: Arial, "
    "sans-serif; margin: 20px;"
    "} h1 { text-align: center; } table { border-collapse: collapse; margin: "
    "20px auto; }"
    "th, td { border: 1px solid #ccc; padding: 10px; } a { text-decoration: "
    "none; } </style>"
    "</head> <body> <h1>Directory Listing</h1> <table> <thead> <tr> "
    "<th>Name</th> <th>Last Modified</th>"
    "<th>Size</th> <th>Type</th>  </tr> </thead> <tbody>";

static inline std::string create_list_dir_entry(const std::string &path) {
  std::ostringstream oss;
  char m_time[30] = {0};
  oss << "<tr><td><a href=\"" << path << ">" << path << "</a></td><td>";
  struct stat sb;
  if (stat(path.c_str(), &sb) < 0) {
    // TODO: Handle failure of the stat. What does that mean?
  }
  strftime(m_time, sizeof(m_time), "%y-%b-%d %H:%M:%S",
           std::localtime(&(sb.st_mtime)));
  oss << std::string(m_time) << "</td><td>";
  switch (sb.st_mode & S_IFMT) {
  case S_IFREG:
    oss << sb.st_size << " KB</td><td>File</td></tr>";
    break;
  case S_IFDIR:
    // TODO: handle directory
    break;
  }

  return oss.str();
  ;
}

std::string HTTPResponse::create_list_dir_() {
  std::ostringstream oss;
  DIR *dir = opendir(".");
  if (dir == NULL) {
    return oss.str();
  }
  oss << list_dir_head;
  struct dirent *dp = NULL;
  while ((dp = readdir(dir))) {
    std::string dir_name = dp->d_name;
    if (dir_name != ".") {
      continue;
    } else {
    }
  }
  oss << "</tbody></table></body></html>";
  return oss.str();
}

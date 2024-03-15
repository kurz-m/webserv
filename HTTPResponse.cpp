#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "HTTPResponse.hpp"
#include "Settings.hpp"

static const std::string proto_ = "HTTP/1.1";

HTTPResponse::HTTPResponse(const ServerBlock &config) : HTTPBase(config), status_code_(0) {
  root_ = config_.find(Token::ROOT).str_val;
}

HTTPResponse::~HTTPResponse() {}

HTTPResponse::HTTPResponse(const HTTPResponse &cpy)
    : HTTPBase(cpy), status_code_(cpy.status_code_), root_(cpy.root_), uri_(cpy.uri_) {}

HTTPResponse &HTTPResponse::operator=(const HTTPResponse &other) {
  HTTPBase::operator=(other);
  if (this != &other) {
    status_code_ = other.status_code_;
    root_ = other.root_;
    uri_ = other.uri_;
  }
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
    uri_ = root_ + "/" + curr_conf.find(Token::INDEX).str_val;
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

uint8_t HTTPResponse::check_uri_() {
  struct stat sb;
  // std::string uri_ = req.parsed_header_.at("URI");
  const RouteBlock *route = config_.find(uri_);

  // TODO: make better function to check for CGI
  if (uri_.find("/cgi-bin") != std::string::npos) {
    return CGI;
  }
  uri_ = root_ + "/" + uri_;
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

// Socket::status HTTPResponse::check_

Socket::status HTTPResponse::prepare_for_send(HTTPRequest &req) {
  uri_ = req.parsed_header_.at("URI");
  uint8_t mask = check_uri_();
  std::ifstream file;
  switch (mask) {
  case CGI:
    return call_cgi_(req);
  case (DIRECTORY | AUTO): // -> list dir
    // TODO: create function for index_of
    body_ = create_list_dir_();
    std::cout << body_ << std::endl;
    status_code_ = 200;
    break;
  case (DIRECTORY): // -> 403
    // Return 403
    status_code_ = 403;
    body_.assign(create_status_html(status_code_));
    break;
  case (DIRECTORY | INDEX): // -> index.html file
  case (FIL):               // -> send file
    // check if file exists
    file.open(uri_.c_str());
    if (file.is_open()) {
      status_code_ = 200;
      read_file_(file);
    } else {
      status_code_ = 404;
      body_.assign(create_status_html(status_code_));
    }
    break;
  case (FAIL): // -> 404
    status_code_ = 404;
    body_.assign(create_status_html(status_code_));
    break;
  }
  make_header_();
  return Socket::READY;
}

Socket::status HTTPResponse::call_cgi_(HTTPRequest &req) {
  std::string ret = "";
  if (access((root_ + uri_).c_str(), F_OK) != 0) {
    status_code_ = 404;
    body_.assign(create_status_html(status_code_));
    make_header_();
    return Socket::READY;
  } else if (access((root_ + uri_).c_str(), X_OK) != 0) {
    std::ifstream file((root_ + uri_).c_str());
    read_file_(file);
    status_code_ = 200;
    make_header_();
    return Socket::READY;
  }
  create_pipe_(req);
  return check_child_status();
}

Socket::status HTTPResponse::check_child_status() {
  int stat_loc = 0;
  pid_t pid_check = waitpid(cgi_pid_, &stat_loc, WNOHANG);
  if (pid_check == 0) {
    // child still running. 
    // TODO: set EVENTS to 0!
    return Socket::WAITCGI;
  } else if (pid_check < 0) {
    status_code_ = 500;
    body_.assign(create_status_html(status_code_));
    make_header_();
    return Socket::READY;
  } 
  else {
    if (WIFEXITED(stat_loc)) {
      status_code_ = 200;
      read_child_pipe_();
      make_header_();
    } else {
      status_code_ = 500;
      body_.assign(create_status_html(status_code_));
      make_header_();
    }
    return Socket::READY;
  }
}

void HTTPResponse::read_child_pipe_() {
  std::cout << "child exited. trying to read the pipe" << std::endl;
  char buffer[BUFFER_SIZE + 1] = {0};
  while (read(child_pipe_, buffer, BUFFER_SIZE) > 0) {
    body_ += buffer;
    std::memset(buffer, 0, sizeof(buffer));
  }
  close(child_pipe_);
}

void HTTPResponse::create_pipe_(HTTPRequest &req) {
  int pipe_fd[2];
  std::string ret = "";

  if (pipe(pipe_fd) < 0)
    throw std::runtime_error(std::strerror(errno));
  cgi_pid_ = fork();
  if (cgi_pid_ == -1)
    throw std::runtime_error(std::strerror(errno));
  if (cgi_pid_ == 0) {
    std::cout << "child executing ... " << std::endl;
    if (dup2(pipe_fd[1], STDOUT_FILENO) < 0)
      throw std::runtime_error(std::strerror(errno));
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    execute_(req);
    std::exit(0);
  } else {
    close(pipe_fd[1]);
    child_pipe_ = pipe_fd[0];
  }
}

// https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script
void HTTPResponse::execute_(HTTPRequest &req) {
  std::vector<std::string> tmp_env;
  std::string exec;
  char *env[6];
  char *argv[2];

  size_t pos = uri_.find("?");
  if (pos != std::string::npos) {
    exec = root_ + uri_.substr(0, pos);
    tmp_env.push_back("QUERY_STRING=" + uri_.substr(pos + 1));
  } else {
    exec = root_ + uri_.substr(0);
  }

  tmp_env.push_back("CONTENT_TYPE=text/html");
  //  req_.parsed_header_.at("Content-Type"));
  tmp_env.push_back("SCRIPT_NAME=" + uri_);
  if (req.method_ == GET) {
    tmp_env.push_back("REQUEST_METHOD=GET");
  } else if (req.method_ == POST) {
    tmp_env.push_back("REQUEST_METHOD=POST");
    std::ostringstream oss;
    oss << "CONTENT_LENGTH=" << req.body_.size();
    tmp_env.push_back(oss.str());
  }
  size_t i = 0;
  for (i = 0; i < tmp_env.size(); ++i) {
    env[i] = const_cast<char *>(tmp_env.at(i).c_str());
  }
  env[i] = NULL;
  argv[0] = const_cast<char *>(exec.c_str());
  argv[1] = NULL;
  execve(exec.c_str(), argv, env);
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

struct FileInfo {
  std::string name;
  bool is_dir;
};

static inline FileInfo create_list_dir_entry(const std::string &path) {
  FileInfo file;
  std::ostringstream oss;
  char m_time[30] = {0};
  oss << "<tr><td><a href=\"" << path << "\">" << path << "</a></td><td>";
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
    file.is_dir = false;
    break;
  case S_IFDIR:
    oss << "</td><td>Directory</td></tr>";
    file.is_dir = true;
    break;
  }

  file.name = oss.str();
  return file;
}

inline bool compare_file(const FileInfo& a, const FileInfo& b) {
  if (a.is_dir && b.is_dir) {
    return a.name < b.name;
  } else {
    return a.is_dir > b.is_dir;
  }
}

std::string HTTPResponse::create_list_dir_() {
  std::vector<FileInfo> files;
  std::ostringstream oss;
  DIR *dir = opendir(uri_.c_str());
  if (dir == NULL) {
    return oss.str();
  }
  oss << list_dir_head;
  struct dirent *dp = NULL;
  while ((dp = readdir(dir))) {
    std::string dir_name = dp->d_name;
    if (dir_name == ".") {
      continue;
    } else {
      files.push_back(create_list_dir_entry(dp->d_name));
    }
  }
  std::sort(files.begin(), files.end(), compare_file);
  std::vector<FileInfo>::iterator it;
  for (it = files.begin(); it != files.end(); ++it) {
    oss << it->name;
  }
  oss << "</tbody></table></body></html>";
  return oss.str();
}

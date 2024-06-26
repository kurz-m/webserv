#include <algorithm>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "EventLogger.hpp"
#include "HTTPResponse.hpp"
#include "Settings.hpp"

static const std::string proto_ = "HTTP/1.1";

HTTPResponse::HTTPResponse(const ServerBlock &config)
    : HTTPBase(config), status_(), uri_(), cgi_pid_(), child_pipe_(),
      cgi_timestamp_(), killed_childs_(), mime_type_() {
  root_ = config_.root;
}

HTTPResponse::~HTTPResponse() {
  if (killed_childs_.size() > 0) {
    std::vector<pid_t>::const_iterator it;
    for (it = killed_childs_.begin(); it != killed_childs_.end(); ++it) {
      waitpid(*it, NULL, WNOHANG);
    }
  }
}

HTTPResponse::HTTPResponse(const HTTPResponse &cpy)
    : HTTPBase(cpy), root_(cpy.root_), uri_(cpy.uri_), cgi_pid_(cpy.cgi_pid_),
      child_pipe_(cpy.child_pipe_), cgi_timestamp_(cpy.cgi_timestamp_),
      killed_childs_(cpy.killed_childs_), mime_type_(cpy.mime_type_) {}

HTTPResponse &HTTPResponse::operator=(const HTTPResponse &other) {
  HTTPBase::operator=(other);
  if (this != &other) {
    status_ = other.status_;
    root_ = other.root_;
    uri_ = other.uri_;
    cgi_pid_ = other.cgi_pid_;
    child_pipe_ = other.child_pipe_;
    cgi_timestamp_ = other.cgi_timestamp_;
    killed_childs_ = other.killed_childs_;
    mime_type_ = other.mime_type_;
  }
  return *this;
}

/* Enum for defining how to handle the incoming request URI */
enum uri_state {
  DIRECTORY = (1 << 0), /**<URI is pointing to a directory*/
  FIL = (1 << 1),       /**<URI is pointing to a file*/
  FAIL = (1 << 2),      /**<URI is pointing to a non-existing file*/
  AUTO = (1 << 3),      /**<Autoindex is set to on in config file*/
  INDEX = (1 << 4),     /**<Indexfile is set in the config file*/
  CGI = (1 << 5),       /**<URI is pointing to a cgi file*/
};

template <typename T>
uint8_t HTTPResponse::check_list_dir_(const T &curr_conf) {
  if (curr_conf.index.length() > 0) {
    uri_ = "/" + curr_conf.index;
    return DIRECTORY | INDEX;
  }
  switch (curr_conf.autoindex) {
  case 1:
    return DIRECTORY | AUTO;
  default:
    return DIRECTORY;
  }
}

bool ends_with(const std::string &str, const std::string &extension) {
  if (extension.size() > str.size())
    return false;
  return std::equal(extension.rbegin(), extension.rend(), str.rbegin());
}

bool HTTPResponse::check_cgi_() {
  std::string endpoint;
  std::string uri_stem;

  uri_stem = uri_.substr(0, uri_.rfind('/'));
  endpoint =
      uri_.substr(uri_.rfind('/') + 1, uri_.rfind('?') - (uri_.rfind('/') + 1));

  if (uri_stem.find("/cgi-bin") != std::string::npos &&
      (ends_with(endpoint, ".py") || ends_with(endpoint, ".php") ||
       ends_with(endpoint, ".sh"))) {
    return true;
  }
  return false;
}

uint8_t HTTPResponse::check_uri_() {
  struct stat sb;
  const RouteBlock *route = config_.find(uri_);
  std::string endpoint;
  std::string uri_stem;

  uri_stem = uri_.substr(0, uri_.rfind('/'));
  endpoint =
      uri_.substr(uri_.rfind('/') + 1, uri_.rfind('?') - (uri_.rfind('/') + 1));

  if (uri_stem.find("/cgi-bin") != std::string::npos &&
      (ends_with(endpoint, ".py") || ends_with(endpoint, ".php") ||
       ends_with(endpoint, ".sh"))) {
    return CGI;
  }
  if (stat((root_ + uri_).c_str(), &sb) < 0) {
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

void HTTPResponse::make_header_(
    const HTTPRequest &req,
    const std::vector<std::string> &extra /*= std::vector<std::string>()*/) {
  std::time_t now = std::time(NULL);
  std::tm *tm = std::gmtime(&now);
  char buffer[80] = {0};
  std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", tm);
  std::ostringstream oss;
  oss << proto_ << " " << status_code_ << " " << status_map_.at(status_code_)
      << CRLF;
  oss << "Date: " << buffer << CRLF;
  oss << "Server: Webserver/0.1 (C++)" << CRLF;
  oss << "Content-Type: " << mime_type_ << CRLF;
  oss << "Content-Length: " << body_.length() << CRLF;
  if (req.keep_alive_ == false) {
    oss << "Connection: close" << CRLF;
  }
  std::vector<std::string>::const_iterator it;
  for (it = extra.begin(); it != extra.end(); ++it) {
    oss << *it << CRLF;
  }
  oss << CRLF;
  buffer_ = oss.str();
  buffer_ += body_;
}

std::string HTTPResponse::get_mime_type_() {
  std::string extension;
  try {
    extension = uri_.substr(0, uri_.find('?'));
  } catch (std::exception &e) {
    extension = uri_;
  }
  try {
    extension = uri_.substr(uri_.rfind('.'));
  } catch (std::exception &e) {
    return "text/html";
  }
  std::string ret;
  try {
    ret = mime_map_.at(extension);
  } catch (std::out_of_range &e) {
    ret = "application/octet-stream";
  }
  return ret;
}

void HTTPResponse::read_file_() {
  LOG_DEBUG((root_ + uri_));
  std::ifstream file((root_ + uri_).c_str(), std::ios_base::binary);
  std::ostringstream oss;
  if (!file.good()) {
    status_code_ = 404;
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    return;
  }
  mime_type_ = get_mime_type_();
  oss << file.rdbuf();
  body_ = oss.str();
  status_code_ = 200;
}

ISocket::status HTTPResponse::get_method_(HTTPRequest &req) {
  uri_ = req.parsed_header_.at("URI");
  uint8_t mask = check_uri_();
  switch (mask) {
  case CGI:
    return call_cgi_(req);
  case (DIRECTORY | AUTO): // -> list dir
    body_ = create_list_dir_();
    status_code_ = 200;
    mime_type_ = "text/html";
    break;
  case (DIRECTORY): // -> 403
    status_code_ = 403;
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    break;
  case (DIRECTORY | INDEX): // -> index.html file
  case (FIL):               // -> send file
    read_file_();
    break;
  case (FAIL): // -> 404
    status_code_ = 404;
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    break;
  }
  // LOG_DEBUG("Body: " + body_);
  make_header_(req);
  return ISocket::READY_SEND;
}

ISocket::status HTTPResponse::delete_method_(const HTTPRequest &req) {
  if (remove((root_ + uri_).c_str())) {
    status_code_ = 404;
    body_.assign(create_status_html(status_code_, "Requested File not found!"));
  } else {
    status_code_ = 200;
    body_.assign(create_status_html(status_code_, "File deleted"));
  }
  mime_type_ = "text/html";
  make_header_(req);
  return ISocket::READY_SEND;
}

ISocket::status HTTPResponse::put_method_(HTTPRequest &req) {
  std::string filename = root_ + uri_;
  if (access((filename).c_str(), F_OK) == 0) {
    status_code_ = 200;
  } else {
    status_code_ = 201;
  }
  std::ofstream file(filename.c_str(), std::ios::trunc | std::ios::binary);
  if (!file.is_open()) {
    status_code_ = 500;
  } else {
    file.write(req.body_.c_str(), req.body_.size());
  }
  body_.assign(create_status_html(status_code_));
  mime_type_ = "text/html";
  make_header_(req);
  return ISocket::READY_SEND;
}

ISocket::status HTTPResponse::post_method_(HTTPRequest &req) {
  if (check_cgi_()) {
    return call_cgi_(req);
  }
  std::vector<std::string> extra;
  std::string endpoint =
      uri_.substr(uri_.rfind('/') + 1, uri_.rfind('?') - (uri_.rfind('/') + 1));
  std::string filename = root_ + "/upload/" + endpoint;
  if (access((filename).c_str(), F_OK) == 0) {
    status_code_ = 303;
    extra.push_back("Location: /upload/" + endpoint);
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    make_header_(req, extra);
  } else {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
      status_code_ = 500;
    } else {
      status_code_ = 201;
      file.write(req.body_.c_str(), req.body_.size());
      extra.push_back("Location: /upload/" + endpoint);
    }
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    make_header_(req, extra);
  }
  return ISocket::READY_SEND;
}

template <typename T>
method_e HTTPResponse::check_method_(const T &curr_conf, method_e method) {
  switch (method) {
  case GET:
  case POST:
  case PUT:
  case DELETE:
    return (method & curr_conf.allow) ? method : FORBIDDEN;
  default:
    return method;
  }
}

ISocket::status HTTPResponse::prepare_for_send(HTTPRequest &req) {
  size_t body_size = req.tbr_ - req.header_.length();
  if (body_size > config_.client_max_body_size) {
    status_code_ = 413;
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    make_header_(req);
    return ISocket::READY_SEND;
  }
  uri_ = req.parsed_header_.at("URI");
  LOG_INFO("Request Method: " + print_method(req.method_));
  const RouteBlock *route = config_.find(uri_);
  method_e method = req.method_;
  if (route) {
    method = check_method_(*route, req.method_);
  } else {
    method = check_method_(config_, req.method_);
  }
  if (!route->redir.empty()) {
    std::vector<std::string> extra;
    status_code_ = 303;
    extra.push_back("Location: " + route->redir);
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    make_header_(req, extra);
    return ISocket::READY_SEND;
  }
  switch (method) {
  case GET:
    return get_method_(req);
  case POST:
    return post_method_(req);
  case PUT:
    return put_method_(req);
  case DELETE:
    return delete_method_(req);
  case FORBIDDEN:
    status_code_ = 403;
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    make_header_(req);
    return ISocket::READY_SEND;
  default:
    status_code_ = 501;
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    make_header_(req);
    return ISocket::READY_SEND;
  }
}

bool HTTPResponse::check_cgi_timeout_() {
  return std::difftime(std::time(NULL), cgi_timestamp_) > CGI_TIMEOUT;
}

ISocket::status HTTPResponse::call_cgi_(HTTPRequest &req) {
  std::string exec = uri_.substr(0, uri_.find("?"));
  if (access((root_ + exec).c_str(), F_OK) != 0) {
    status_code_ = 404;
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    make_header_(req);
    return ISocket::READY_SEND;
  } else if (access((root_ + exec).c_str(), X_OK) != 0) {
    status_code_ = 403;
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    make_header_(req);
    return ISocket::READY_SEND;
  }
  create_pipe_(req);
  cgi_timestamp_ = std::time(NULL);
  return check_child_status(req);
}

ISocket::status HTTPResponse::check_child_status(const HTTPRequest &req) {
  if (check_cgi_timeout_()) {
    return kill_child_(req);
  }
  int stat_loc = 0;
  pid_t pid_check = waitpid(cgi_pid_, &stat_loc, WNOHANG);
  if (pid_check == 0) {
    return ISocket::WAITCGI;
  } else if (pid_check < 0) {
    status_code_ = 500;
    body_.assign(create_status_html(status_code_));
    mime_type_ = "text/html";
    make_header_(req);
    return ISocket::READY_SEND;
  } else {
    if (WIFEXITED(stat_loc)) {
      status_code_ = 200;
      read_child_pipe_();
      mime_type_ = "text/html";
      LOG_DEBUG(buffer_);
      make_header_(req);
    } else {
      status_code_ = 500;
      body_.assign(create_status_html(status_code_));
      mime_type_ = "text/html";
      make_header_(req);
    }
    return ISocket::READY_SEND;
  }
}

ISocket::status HTTPResponse::kill_child_(const HTTPRequest &req) {
  if (child_pipe_ > 0) {
    close(child_pipe_);
    child_pipe_ = -1;
  }
  kill(cgi_pid_, SIGKILL);
  pid_t pid = waitpid(cgi_pid_, NULL, WNOHANG);
  if (pid == 0) {
    /* await these childs in the destructor. */
    killed_childs_.push_back(cgi_pid_);
  }
  cgi_pid_ = 0;
  status_code_ = 500;
  body_ = create_status_html(500);
  mime_type_ = "text/html";
  make_header_(req);
  return ISocket::READY_SEND;
}

void HTTPResponse::read_child_pipe_() {
  LOG_DEBUG("child exited. trying to read the pipe");
  char buffer[BUFFER_SIZE + 1] = {0};
  /* FIX: check if this is blocking or not? */
  while (read(child_pipe_, buffer, BUFFER_SIZE) > 0) {
    body_ += buffer;
    std::memset(buffer, 0, sizeof(buffer));
  }
  close(child_pipe_);
}

void HTTPResponse::create_pipe_(HTTPRequest &req) {
  int pipe_fd[2];
  int pipe_fd2[2];
  size_t tw = req.body_.size();
  ;
  std::string ret = "";

  if (pipe(pipe_fd) < 0) {
    return;
  }
  if (pipe(pipe_fd2) < 0) {
    goto leave_first;
  }
  if ((child_pipe_ = dup(pipe_fd[0])) < 0) {
    goto leave_second;
  }
  cgi_pid_ = fork();
  if (cgi_pid_ == -1) {
    goto leave_second;
  }
  if (cgi_pid_ == 0) {
    LOG_DEBUG("child executing ... ");
    if (dup2(pipe_fd[1], STDOUT_FILENO) < 0 ||
        dup2(pipe_fd2[0], STDIN_FILENO) < 0)
      throw std::runtime_error(std::strerror(errno));
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    close(pipe_fd2[0]);
    close(pipe_fd2[1]);
    execute_(req);
    std::exit(0);
  }
  fcntl(pipe_fd2[1], F_SETFL, O_NONBLOCK);
  while ((tw -= write(pipe_fd2[1], req.body_.c_str(), req.body_.size())) > 0)
    ;

leave_second:
  close(pipe_fd2[0]);
  close(pipe_fd2[1]);

leave_first:
  close(pipe_fd[0]);
  close(pipe_fd[1]);
}

// https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script
void HTTPResponse::execute_(HTTPRequest &req) {
  std::vector<std::string> tmp_env;
  std::string exec;
  char *env[6];
  char *argv[3];

  size_t pos = uri_.find("?");
  if (pos != std::string::npos) {
    exec = root_ + uri_.substr(0, pos);
    tmp_env.push_back("QUERY_STRING=" + uri_.substr(pos + 1));
  } else {
    exec = root_ + uri_.substr(0);
  }

  tmp_env.push_back("CONTENT_TYPE=text/html");
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
  if (ends_with(exec, ".py")) {
    argv[0] = const_cast<char *>("python");
    argv[1] = const_cast<char *>(exec.c_str());
  } else if (ends_with(exec, ".php")) {
    argv[0] = const_cast<char *>("php");
    argv[1] = const_cast<char *>(exec.c_str());
  } else {
    argv[0] = const_cast<char *>("exit");
    argv[1] = const_cast<char *>("1");
  }
  argv[2] = NULL;
  LOG_DEBUG("Exec: " + exec);
  execve(exec.c_str(), argv, env);
}

/**
 * Constant std::string for having the html file for listing directories.
 *
 * This variable is for creating the html page when autoindex in on and no index
 * page is provided. It is the start of the list-dir.html which will be filled
 * up later with the files and folders from the requested directory.
 */
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

/**
 * Helper struct for creating the list dir entries and sorting them.
 *
 * This struct is defined for being used within the creation of the list dir
 * functions. It contains the name of the file and a check if it is a directory
 * or not.
 */
struct FileInfo {
  std::string name; /**< Name of the file. */
  bool is_dir;      /**< True if it is a directory, otherwise false. */
};

/**
 * Static function for creating a single entry for list dir.
 *
 * This function is used to create a single entry for the listing of a
 * directory. It creates the appropriate string and also determines if the
 * checked file is file or a directory.
 *
 * \param root Const reference to the webroot.
 * \param uri Const reference to the requested URI.
 * \param path Const reference to the path.
 *
 * \return FileInfo struct.
 */
static FileInfo create_list_dir_entry(const std::string &root,
                                      const std::string &uri,
                                      const std::string &path) {
  FileInfo file;
  std::string full_path = root + uri + "/" + path;
  std::string link_href = uri + "/" + path;
  if (uri == "/") {
    link_href = uri + path;
  }
  std::ostringstream oss;
  char m_time[30] = {0};
  oss << "<tr><td><a href=\"" << link_href << "\">" << path << "</a></td><td>";
  struct stat sb;
  if (stat(full_path.c_str(), &sb) < 0) {
    LOG_WARNING("Failed stat() within create_list_dir_entry");
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

/**
 * Local helper function for sorting the dir listing folders first.
 */
bool compare_file(const FileInfo &a, const FileInfo &b) {
  if (a.is_dir && b.is_dir) {
    return a.name < b.name;
  } else {
    return a.is_dir > b.is_dir;
  }
}

std::string HTTPResponse::create_list_dir_() {
  std::vector<FileInfo> files;
  std::ostringstream oss;
  DIR *dir = opendir((root_ + uri_).c_str());
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
      files.push_back(create_list_dir_entry(root_, uri_, dir_name));
    }
  }
  std::sort(files.begin(), files.end(), compare_file);
  std::vector<FileInfo>::iterator it;
  for (it = files.begin(); it != files.end(); ++it) {
    oss << it->name;
  }
  oss << "</tbody></table></body></html>";
  closedir(dir);
  return oss.str();
}

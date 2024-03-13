#include "CGI.hpp"
#include <cerrno>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <sys/wait.h>

std::string CGI::call_cgi(const std::string &uri,
                          const HTTPResponse &response) {
  CGI instance(uri, response);
  std::string ret = "";
  pid_t pid = instance.create_pipe_();
  waitpid(pid, NULL, 0); // TODO: make this not block other requests!
  char buffer[1024];
  while(read(STDIN_FILENO, buffer, 1024) > 0) {
    ret += buffer;
  }
  return ret;
}

// https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script
void CGI::prepare_env_() {
  env_str_.push_back("CONTENT_TYPE=" +
                     response_.request_.parsed_header_.at("Content-Type"));
  env_str_.push_back("SCRIPT_NAME=" + response_.request_.parsed_header_.at("URI"));
  if (response_.request_.method_ == GET) {
    env_str_.push_back("REQUEST_METHOD=GET");
  } else if (response_.request_.method_ == POST) {
    env_str_.push_back("REQUEST_METHOD=POST");
    std::ostringstream oss;
    oss << "CONTENT_LENGTH=" << response_.request_.body_.size();
    env_str_.push_back(oss.str());
  }
  size_t i = 0;
  for (i = 0; i < env_str_.size(); ++i) {
    env_[i] = const_cast<char*>(env_str_.at(i).c_str());
  }
  env_[i] = NULL;
}

void CGI::execute_() {
  parse_uri_();
  prepare_env_();
  char * argv[2];
  argv[0] = const_cast<char*>(exec_.c_str());
  argv[1] = NULL;
  execve(exec_.c_str(), argv, env_);
}

void CGI::parse_uri_() {
  size_t pos = 0;
  pos = uri_.find("?");
  if (pos != std::string::npos) {
    exec_ = uri_.substr(0, pos);
    env_str_.push_back("QUERY_STRING=" + uri_.substr(pos + 1));
  } else {
    exec_ = uri_.substr(0);
  }
}

pid_t CGI::create_pipe_() {
  pid_t pid;
  int pipe_fd[2];

  if (pipe(pipe_fd) < 0)
    throw std::runtime_error(std::strerror(errno));
  pid = fork();
  if (pid == -1)
    throw std::runtime_error(std::strerror(errno));
  if (pid == 0) {
    if (dup2(pipe_fd[1], STDOUT_FILENO) < 0)
      throw std::runtime_error(std::strerror(errno));
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    execute_();
  } else {
    if (dup2(pipe_fd[0], STDIN_FILENO) < 0)
      throw std::runtime_error(std::strerror(errno));
    close(pipe_fd[0]);
    close(pipe_fd[1]);
  }
  return (pid);
}

CGI::CGI(const std::string &uri, const HTTPResponse &response)
    : uri_(uri), response_(response) {}

CGI::~CGI() {}

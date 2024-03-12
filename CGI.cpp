#include "CGI.hpp"
#include <cerrno>
#include <cstring>
#include <sstream>
#include <stdexcept>

std::string CGI::call_cgi(const std::string &uri,
                          const HTTPResponse &response) {
  CGI instance(uri, response);
  instance.parse_uri_();
  instance.create_pipe_();
}

// https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script
void CGI::prepare_env_() {
  if (response_.request_.method_ == GET) {
    // put REQUEST_METHOD var into env_str_
  } else if (response_.request_.method_ == POST) {
    // also put sdlfja
  }
}

void CGI::execute_() { execve(exec_.c_str(), NULL, env_); }

void CGI::parse_uri_() {
  size_t pos = 0;
  pos = uri_.find("?");
  if (pos != std::string::npos) {
    exec_ = uri_.substr(0, pos);
    query_str_ = uri_.substr(pos + 1);
  } else {
    exec_ = uri_.substr(0);
  }
}

// pid_t	create_pipe(void (f1)(void *), void *a1)
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

#include "CGI.hpp"
#include <cerrno>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <sys/wait.h>
#include <iostream>
#include <cstdlib>

std::string CGI::call_cgi(const std::string &uri,
                          const HTTPRequest &req) {
  CGI instance(uri, req);
  std::string ret = "";
  if (access((req.config_.find(Token::ROOT).str_val + uri).c_str(), (F_OK | X_OK)) != 0)
  {
    // read file!
  }
  return instance.create_pipe_();   
}

// https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script
void CGI::prepare_env_() {
  env_str_.push_back("CONTENT_TYPE=text/html");
                    //  req_.parsed_header_.at("Content-Type"));
  env_str_.push_back("SCRIPT_NAME=" + req_.parsed_header_.at("URI"));
  if (req_.method_ == GET) {
    env_str_.push_back("REQUEST_METHOD=GET");
  } else if (req_.method_ == POST) {
    env_str_.push_back("REQUEST_METHOD=POST");
    std::ostringstream oss;
    oss << "CONTENT_LENGTH=" << req_.body_.size();
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
    exec_ = req_.config_.find(Token::ROOT).str_val + uri_.substr(0, pos);
    env_str_.push_back("QUERY_STRING=" + uri_.substr(pos + 1));
  } else {
    exec_ = req_.config_.find(Token::ROOT).str_val + uri_.substr(0);
  }
}

std::string CGI::create_pipe_() {
  pid_t pid;
  int pipe_fd[2];
  std::string ret = "";

  if (pipe(pipe_fd) < 0)
    throw std::runtime_error(std::strerror(errno));
  pid = fork();
  if (pid == -1)
    throw std::runtime_error(std::strerror(errno));
  if (pid == 0) {
    std::cout << "child executing ... " << std::endl;
    if (dup2(pipe_fd[1], STDOUT_FILENO) < 0)
      throw std::runtime_error(std::strerror(errno));
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    execute_();
    std::exit(0);
  } else {
    // if (dup2(pipe_fd[0], STDIN_FILENO) < 0)
    //   throw std::runtime_error(std::strerror(errno));
    // close(pipe_fd[0]);
    close(pipe_fd[1]);
    int stat_loc = 0;
    pid = waitpid(pid, &stat_loc, 0);
    // while (pid == 0) {
    //   std::cout << "waiting for child..." << std::endl;
    //   usleep(1000000);
    //   pid = waitpid(pid, NULL, WNOHANG);
    // }
    std::cout << "child exited. trying to read the pipe" << std::endl;
    char buffer[1024] = {0};
    while(read(pipe_fd[0], buffer, 1023) > 0) {
      ret += buffer;
      memset(buffer, 0, sizeof(buffer));
    }
    close(pipe_fd[0]);
  }
  return (ret);
}

CGI::CGI(const std::string &uri, const HTTPRequest &req)
    : uri_(uri), req_(req) {}

CGI::~CGI() {}

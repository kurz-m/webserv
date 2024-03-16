#ifndef __HTTPREQUEST_HPP__
#define __HTTPREQUEST_HPP__

#include "HTTPBase.hpp"
#include "Settings.hpp"
#include "Socket.hpp"

class HTTPRequest : public HTTPBase {
public:
  HTTPRequest(const ServerBlock &);
  HTTPRequest(const HTTPRequest &cpy);
  HTTPRequest &operator=(const HTTPRequest &other);
  ~HTTPRequest();

  ISocket::status parse_header();
  void parse_body();

private:
  std::map<std::string, std::string> parsed_header_;
  method_e method_;

  friend class Socket;
  friend class HTTPResponse;
};

#endif // __HTTPREQUEST_HPP__

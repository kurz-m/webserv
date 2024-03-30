#ifndef __HTTPREQUEST_HPP
#define __HTTPREQUEST_HPP

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
  size_t tbr_;
  bool header_parsed_;
  bool keep_alive_;

  friend class SocketConnect;
  friend class HTTPResponse;
};

#endif /* HTTPRequest.hpp */

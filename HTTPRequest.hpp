#ifndef __HTTPREQUEST_HPP__
#define __HTTPREQUEST_HPP__

#include "HTTPBase.hpp"

class Socket;

class HTTPRequest : public HTTPBase {
public:
  HTTPRequest();
  HTTPRequest(const HTTPRequest &cpy);
  HTTPRequest &operator=(const HTTPRequest &other);
  ~HTTPRequest();

  void parse_header(Socket &sock);
  void parse_body();

private:

friend class Socket;
};

#endif // __HTTPREQUEST_HPP__

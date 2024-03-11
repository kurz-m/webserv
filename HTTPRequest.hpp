#ifndef __HTTPREQUEST_HPP__
#define __HTTPREQUEST_HPP__

#include "Settings.hpp"
#include "Socket.hpp"
#include "HTTPBase.hpp"

class HTTPRequest : public HTTPBase {
public:
  HTTPRequest(const ServerBlock&);
  HTTPRequest(const HTTPRequest &cpy);
  HTTPRequest &operator=(const HTTPRequest &other);
  ~HTTPRequest();

  Socket::status parse_header();
  void parse_body();

private:

friend class Socket;
};

#endif // __HTTPREQUEST_HPP__

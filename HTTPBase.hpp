#ifndef __HTTP_BASE_HPP
#define __HTTP_BASE_HPP

#include <map>
#include <string>

#include "HTTP.hpp"
#include "Settings.hpp"
#include "Socket.hpp"

class HTTPBase {
public:
  HTTPBase(const ServerBlock &);
  HTTPBase(const HTTPBase &cpy);
  HTTPBase &operator=(const HTTPBase &other);
  virtual ~HTTPBase();

  static const char *status_code[];

  void reset();
  std::string create_status_html(int status_code,
                                 const std::string &message = "") const;

protected:
  void parse_buffer_();

  std::string buffer_;
  std::string body_;
  std::string header_;
  int status_code_;
  const ServerBlock &config_; // all settings from config

  const static std::map<int, std::string> status_map_;
  const static std::map<std::string, std::string> mime_map_;
  const static size_t MAX_BUFFER = 1 * 1024 * 1024;

  friend class SocketConnect;
  friend class Server;
};

#endif /* HTTPBase.hpp */

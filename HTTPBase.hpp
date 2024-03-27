#ifndef __HTTP_BASE__
#define __HTTP_BASE__

#include "HTTP.hpp"
#include "Settings.hpp"
#include "Socket.hpp"
#include <map>
#include <string>

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

  static std::map<int, std::string> create_map_();
  const static std::map<int, std::string> status_map_;
  const static size_t MAX_BUFFER = 1024;
  const ServerBlock &config_; // all settings from config

  friend class SocketConnect;
  friend class Server;
};

#endif // __HTTP_BASE__

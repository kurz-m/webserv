#ifndef __SERVER_HPP
#define __SERVER_HPP

#include <list>
#include <map>
#include <sys/poll.h>
#include <unistd.h>
#include <vector>

#include "ISocket.hpp"
#include "Parser.hpp"
#include "Settings.hpp"
#include "Socket.hpp"

#define RESET 0

typedef struct sigaction sigaction_t;

class Server {
public:
  Server(const HttpBlock &config);
  ~Server();

  void startup();
  void run();
  void stop();

private:
  int do_poll_();
  void event_handler_();
  void create_listen_socket_(const ServerBlock &config);

  std::list<pollfd_t> poll_list_;
  std::map<int, ISocket> client_map_;
  const HttpBlock &config_; // all settings from config
  static const int poll_timeout_ = 100;

  friend class Socket;
};

#endif /* Server.hpp */

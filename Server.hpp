#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include "Socket.hpp"
#include <map>
#include <list>
#include <sys/poll.h>
#include <unistd.h>
#include <vector>
#include "Settings.hpp"
#include "Parser.hpp"


class Server {
public:
  Server(const HttpBlock &config);
  ~Server();

  void startup();
  void run();

private:
  int  do_poll_();
  void event_handler_();
  void create_listen_socket_(const ServerBlock& config);

  std::list<pollfd_t> poll_list_;
  std::map<int, Socket*> client_map_;
  const HttpBlock& config_;  // all settings from config
  int poll_timeout_;

friend class Socket;
};

#endif // __SERVER_HPP__

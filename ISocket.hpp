#ifndef __SOCKET_INTERFACE_HPP
#define __SOCKET_INTERFACE_HPP

#include <list>

#include "Settings.hpp"

typedef struct addrinfo addrinfo_t;
typedef struct pollfd pollfd_t;

#define DEFAULT_TIMEOUT 15

class Socket;

class ISocket {
public:
  /* Need this here because we need it here
     Enum for displaying the current status of the socket.
     The enum helps us to handle the socket accordingly.*/
  enum status {
    PREPARE_SEND, /**<ConnectSocket is ready for preparing the response*/
    READY_SEND,   /**<ConnectSocket is ready to send the response*/
    READY_RECV,   /**<Listen: ready to accept connection.
                      Connect: ready to receive a request.*/
    URECV,        /**<ConnectSocket has not yet fully received the request*/
    USEND,        /**<ConnectSocket has not yet fully send the response*/
    WAITCGI,      /**<ConnectSocket is waiting for the CGI child*/
    CLOSED,       /**<Socket is being closed for multiple reasons
                      Connect -> timeout, Listen -> connection error*/
  };

  ISocket(pollfd &pollfd, const ServerBlock &config, const addrinfo_t &info);
  ISocket(pollfd_t &pollfd, const ServerBlock &config,
          int timeout = DEFAULT_TIMEOUT);

  ISocket(const ISocket &obj);
  ISocket &operator=(const ISocket &obj);
  ~ISocket();

  status handle(std::map<int, ISocket> &sock_map,
                std::list<pollfd_t> &poll_list);
  bool check_timeout_() const;

private:
  Socket *sock_;

  friend class Server;
};

#endif /* ISocket.hpp */

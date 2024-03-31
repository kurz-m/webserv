#ifndef __SOCKET_HPP
#define __SOCKET_HPP

#include <arpa/inet.h>
#include <ctime>
#include <list>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "ISocket.hpp"

class Server;
class ISocket;

/**
 * A base class for SocketListen and SocketConnect.
 *
 * This class serves as a base class for the sockets in the webserver.
 * It holds the member attributes shared by the 2 socket types.
 * Furthermore, it specifies virtual member functions which
 * both the SocketListen and SocketConnect has to implement. Last, it
 * works as a ptr within ISocket, so we can call the functions independent of
 * the socket type.
 */
class Socket {
public:
  /**
   * Parameterized constructor for the base Socket class.
   *
   * This constructor is taking a reference to a pollfd_t struct living in
   * the pollfds list of the server. This way we can directly access the list
   * elements and can also change and adapt the events we want for the next call
   * to poll.
   *
   * \param pollfd Reference to a pollfd_t struct.
   * \param config Const reference to a ServerBlock.
   */
  Socket(pollfd_t &pollfd, const ServerBlock &config);

  /**
   * Copy constructor for the base Socket class.
   *
   * Constructor takes a const reference to another base Socket instance and
   * copies all the elements to a new Socket, thereby making a deep copy.
   *
   * \param other Const reference to a Socket.
   */
  Socket(const Socket &other);

  /**
   * Copy assignment operator of the base Socket class.
   *
   * \param other Const reference to a Socket.
   */
  Socket &operator=(const Socket &other);

  /**
   * Destructor of the base Socket class as pure virtual.
   *
   * Define the destructor of the base class as pure virtual to let the derived
   * classes define it themselves.
   */
  virtual ~Socket() = 0;

  /**
   * A pure virtual member function to check the Socket timeout.
   *
   * \return true or false if a socket timed out, respectively.
   *
   */
  virtual bool check_timeout_() const = 0;

  /**
   * Pure virtual function to create a new Socket.
   *
   * \sa SocketListen::clone()
   * \sa SocketConnect::clone()
   */
  virtual Socket *clone() const = 0;

  /**
   * Pure virtual function to handle the poll events.
   *
   * \param sock_map Map with pairs of sockfd and corresponding Socket.
   * \param poll_list List with the pollfd_t structs.
   *
   * \sa SocketListen::handle()
   * \sa SocketConnect::handle()
   * \return Current status of the Socket.
   */
  virtual ISocket::status handle(std::map<int, ISocket> &sock_map,
                                 std::list<pollfd_t> &poll_list) = 0;

protected:
  /**
   * Protected const reference to a ServerBlock variable.
   *
   * This is a reference to the ServerBlock parsed from the config file.
   * Each Socket gets its respective ServerBlock with the specific
   * configurations.
   */
  const ServerBlock &config_;

  /**
   * Reference to the pollfd_t in the pollfd_t list of the Server.
   *
   * This reference is used to directly manipulate the pollfd_t struct that is
   * defining the current Socket, e.g. setting the event flags we want etc.
   * \sa Server::poll_list_
   */
  pollfd_t &pollfd_;

  /**
   * Current status of the Socket.
   *
   * This status is important because it enables the webserver to decide what
   * kind of action it has to take. For more details see the enum in the
   * ISocket.
   *
   * \sa ISocket::status
   */
  ISocket::status status_;

  /**
   * Protected function to check for poll error.
   *
   * This function is used for checking for occuring poll errors in the revents
   * field of the pollfd_t struct.
   */
  void check_poll_err_();

  friend class Server;
  friend class HTTPRequest;
  friend class ISocket;
};

#endif /* Socket.hpp */

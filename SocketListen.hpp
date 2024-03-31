#ifndef __SOCKET_LISTEN_HPP
#define __SOCKET_LISTEN_HPP

#include "ISocket.hpp"
#include "Server.hpp"
#include "Socket.hpp"

/**
 * Class for a listening socket derived from Socket.
 *
 * The SocketListen class is created by the call to socket() from the
 * Server. It handles accepting new connection to the server.
 */
class SocketListen : public Socket {
public:
  /**
   * Parameterized constructor for the SocketListen class.
   *
   * This constructor is used as the default constructor taking a reference to
   * its correpsonding pollfd_t element in the pollfd_list from the server and a
   * constant reference to a ServerBlock which defines all its properties.
   * Furthermore, it takes a const reference to an addrinfo_t struct which holds
   * the information regarding the address of a service provider.
   *
   * \param pollfd Reference to its corresponding pollfd element in the
   * pollfd_list of the server.
   * \param config Const reference to a ServerBlock configuration.
   * \param info Const reference to a addrinfo_t struct.
   */
  SocketListen(pollfd &pollfd, const ServerBlock &config,
               const addrinfo_t &info);

  /**
   * Copy constructor for the SocketListen class.
   *
   * Constructor takes a const reference to another SocketListen instance and
   * copies all the elements to a new Socket, thereby making a deep copy.
   *
   * \param other Const reference to a SocketListen instance.
   */
  SocketListen(const SocketListen &cpy);

  /**
   * Copy assignment operator of the SocketListen class.
   *
   * \param other Const reference to a SocketListen instance.
   */
  SocketListen &operator=(const SocketListen &other);

  /**
   * Destructor of the SocketListen class.
   */
  ~SocketListen();

  /**
   * Main entry point for accepting new client connections.
   *
   * If a listening socket gets a POLLIN revent, handle() tries to establish a
   * new connection and adds it directly to the pollfd_list of the server as
   * well as into the key: fd, value: ISocket map. Its status can only be
   * ISocket::READY_RECV because its sole purpose is to establish new client
   * connections.
   */
  ISocket::status handle(std::map<int, ISocket> &client_map,
                         std::list<pollfd_t> &poll_list);

  /**
   * Private member function to check the timeout of the SocketListen.
   *
   * This function always returns false because there is no timeout for the
   * listening sockets.
   *
   * \return Always false.
   */
  bool check_timeout_() const;

  /**
   * Creates a new SocketListen to the ISocket interface.
   *
   * The clone function is used to allocate memory for a new SocketListen
   * instance which will be added into the socket map of the server.
   *
   * \return New SocketListen instance.
   */
  SocketListen *clone() const;

private:
  /**
   * Private member attribute addrinfo_t struct.
   *
   * This private member attribute holds a addrinfo_t struct containing the
   * information regarding the address of the service provider. This includes
   * the protocol family for the socket, length of socket address etc.
   */
  addrinfo_t servinfo_;

  friend class Server;
};

#endif /* SocketListen.hpp */

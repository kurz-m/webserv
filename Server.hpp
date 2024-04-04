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

/**
 * The main class for the webserver..
 *
 * This class is the main entry point to the webserver after the configuration
 * file was parsed. It contains the startup() and run() function which start the
 * webserver.
 */
class Server {
public:
  /**
   * Parameterized constructor for the Server class.
   *
   * This constructor is taking a const reference to a HttpBlock which contains
   * all the parsed configuration from the config file..
   *
   * \param config Const reference to a HttpBlock.
   */
  Server(HttpBlock &config);

  /**
   * Destructor of the Server class.
   */
  ~Server();

  /**
   * Public member function for the webserver.
   *
   * Within the startup function, the server iterates through the servers vector
   * within the HttpBlock to create a new listening socket for each server,
   * therefore calling the create_listen_socket_() function.
   */
  void startup();

  /**
   * Public member function to run the server after startup().
   *
   * The run() function serves the purpose to call the do_poll_() function and
   * the event_handler_() function, which is the main purpose and mechanism of
   * the webserver.
   *
   * \param test Description of the parameter
   * \return Defines the return value
   */
  void run();

private:
  /**
   * Private member function for the single poll call within the webserver.
   *
   * This private member function contains the single poll call used within the
   * I/O multiplexing webserver. It creates a local std::vector<pollfd_t> from
   * the poll_list member. The vector can be used as input for the poll()
   * function using the &vector[0] syntax. After polling, the revents specified
   * in the pollfd of the vector are then copied back to the poll_list
   *
   * \return Amount of events.
   */
  int do_poll_();

  /**
   * Private member function to handle the specified events for each socket.
   *
   * The private member function iterates over the poll_list and calls the
   * ISocket::handle() function which returns the ISocket::status and therefore
   * enables to webserver to check if a socket needs to be closed. At the end,
   * the revents of the poll list are being reset.
   */
  void event_handler_();

  /**
   * Private member Function for creating a listening socket during startup.
   *
   * This private member function creates a listening socket with respect to the
   * specified configurations in the config file.
   *
   * FIX: multiple calls to getaddrinfo. Maybe try to move it into main and call
   * freeaddrinfo after shuding down.
   *
   * \param config Const reference to a ServerBlock.
   */
  void create_listen_socket_(const ServerBlock &config);

  /**
   * Private member attribute of the Server.
   *
   * This std::list serves as a list of pollfd_t structs which hold the fd,
   * events, and revents members. A list is being used because pointers and
   * references keep their validity even after erasing a single element from the
   * list. The Sockets are also always getting a reference to a single element
   * within this list as reference to direcctly manipulate the event member
   * attirubte.
   */
  std::list<pollfd_t> poll_list_;

  /**
   * Private member attribute of the Server.
   *
   * This std::map has a sockfd as key and ISocket interface as value. All
   * sockets are being added to this socket map and defining their lifetime.
   */
  std::map<int, ISocket> sock_map_;

  /**
   * Private reference to a HttpBlock from the configuration file.
   *
   * This private member is a reference to the HttpBlock parsed during
   * startup of the server. It holds all necessary server and route settings..
   *
   * \sa HttpBlock
   */
  HttpBlock &config_;

  /**
   * Private member attribute of the Server.
   *
   * Poll timeout defines the timout that is being used for the poll() call.
   */
  static const int poll_timeout_ = 100;

  friend class Socket;
};

#endif /* Server.hpp */

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <unistd.h>

struct addrinfo *get_addrinfo(struct addrinfo &hints, struct addrinfo *res) {
  char ipstr[INET6_ADDRSTRLEN];
  int status;
  struct addrinfo *p;

  if ((status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
  }

  int len = 0;
  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;

    if (p->ai_family == AF_INET) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
    } else {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
    }

    std::cout << inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr)
              << std::endl;

    len++;
  }

  std::cout << "len = " << len << std::endl;
  
  return res;
}

int get_sock_and_bind(struct addrinfo* res) {
  
  struct addrinfo* p;
  int sockfd;
  int yes = 1;

  for(p = res; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}
  return sockfd;
}

void handle_connection(int sockfd) {
  socklen_t sin_size;
  int new_fd;
  struct sockaddr_storage their_addr; // connector's address information

  sin_size = sizeof(their_addr);
  new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
  if (new_fd == -1) {
    perror("accept");
    return;
  }

  if (!fork()) { // this is the child process
    close(sockfd); // child doesn't need the listener

    char recvline[81];
    int n = 0;

    while ((n = recv(new_fd, recvline, 80, MSG_DONTWAIT)) > 0) {
      std::cout << std::string(recvline);
    }
    std::cout << std::endl;

    std::string buffer = "HTTP/1.0 200 OK\r\n\r\n<html>Hello, World!</html>";
    if (send(new_fd, buffer.c_str(), buffer.size(), 0) == -1)
      perror("send");
    close(new_fd);
    exit(0);
  }
  close(new_fd);  // parent doesn't need this
  return;
}

int main(void) {
  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo, *p;
  char ipstr[INET6_ADDRSTRLEN];
  int sockfd;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM; // TCP not UDP
  hints.ai_flags = AI_PASSIVE;     // Fill in my IP for me

  servinfo = get_addrinfo(hints, servinfo);

  sockfd = get_sock_and_bind(servinfo);
  
  freeaddrinfo(servinfo);

  if (listen(sockfd, 10) == -1) {
    perror("listen");
    exit(1);
  }
  
  struct pollfd sockets[1];

  memset(&sockets, 0, sizeof(sockets));
  sockets[0].fd = sockfd;
  sockets[0].events = POLLIN;

  while (1) {
    int num_events = 0;
    if ((num_events = poll(sockets, 1, 2500)) < 0) {
      perror("poll");
      exit(1);
    }
    if (num_events > 0) {
      handle_connection(sockfd);
    }
    // else {
    //   std::cout << "poll timed out!" << std::endl;
    // }
  }

  return 0;
}
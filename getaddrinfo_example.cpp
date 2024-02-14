#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
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

  if (listen(sockfd, 10) == -1) {
    perror("listen");
    exit(1);
  }

  

  freeaddrinfo(servinfo);
  return 0;
}
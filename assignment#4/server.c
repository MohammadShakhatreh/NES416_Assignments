#include <sys/socket.h>
#include <netinet/in.h>	
#include <arpa/inet.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>

#define	LISTENQ	1024
#define	MAXLINE	8192


/**
* Convert a string to upper case
*/
void upper_case(char * str) {
	size_t len = strlen(str);

	for(int i = 0; i < len; ++i)
		str[i] = toupper(str[i]);
}

void sig_child(int signo) {
	
	int stat;
	pid_t pid;
	
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0);
}

/**
* Handle requests using async I/O
*/
void server_handler(int tcp_fd, int udp_fd) {

	int maxfdp1;
	fd_set rset;
	char buff[MAXLINE], str[128];

	int read_chars;
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);

	FD_ZERO(&rset);
	while(true) {
		FD_SET(tcp_fd, &rset);
		FD_SET(udp_fd, &rset);
		maxfdp1 = (tcp_fd > udp_fd ? tcp_fd : udp_fd ) + 1;

		select(maxfdp1, &rset, NULL, NULL, NULL);

		// TCP connection is ready
		if(FD_ISSET(tcp_fd, &rset)) {

			int connected_fd = accept(tcp_fd, (struct sockaddr *) &cliaddr, &len);

			if(connected_fd < 0) {
				fprintf(stderr, "Error from accept(): (%s)", strerror(errno));
				exit(EXIT_FAILURE);
			}

			printf("Received TCP request from client: %s, port %d\n",
				inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
	 			ntohs(cliaddr.sin_port)
	 		);

			switch (fork()) {
				case -1:
					fprintf(stderr, "Could not create child (%s)", strerror(errno));
					break;

				case 0: // Child
					close(tcp_fd); // Unneeded copy of listening socket
					
					int read_chars;
					if((read_chars = read(connected_fd, buff, MAXLINE)) < 0) {
						fprintf(stderr, "Error from read(): (%s)", strerror(errno));
						exit(EXIT_FAILURE);
					}

					buff[read_chars] = '\0';
					upper_case(buff);
					write(connected_fd, buff, strlen(buff));

					exit(EXIT_SUCCESS);
			}
			close(connected_fd); // Unneeded copy of connected socket
		}

		// UDP datagram is ready to be read
		if(FD_ISSET(udp_fd, &rset)) {

			if((read_chars = recvfrom(udp_fd, buff, MAXLINE, 0,
				(struct sockaddr *) &cliaddr, &len)) < 0) {

				fprintf(stderr, "Error from recvfrom(): (%s)", strerror(errno));
				continue;
			}

			printf("Received UDP request from client: %s, port %d\n",
				inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
	 			ntohs(cliaddr.sin_port)
	 		);

			buff[read_chars] = '\0';
			upper_case(buff);

			sendto(udp_fd, buff, strlen(buff), 0, (struct sockaddr *) &cliaddr, len);
		}
	}
}

/**
* Setup the sockets
*/
int main(int argc, char const *argv[]) {
	if(argc != 2) {
		printf("usage: %s port", argv[0]);
		return EXIT_FAILURE;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));

	// IPv4
	servaddr.sin_family = AF_INET;

	// Make the kernel choose the ip 
	// which is the ip of the PC
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// Port number
	servaddr.sin_port = htons(atoi(argv[1]));

	// Create a TCP IPv4 socket
	int tcp_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(tcp_fd < 0) {
		fprintf(stderr, "Could not create TCP server socket (%s)", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(bind(tcp_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "Error from bind(): %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(listen(tcp_fd, LISTENQ) < 0) {
		fprintf(stderr, "Error from listen(): %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Handle zombie child processes
	signal(SIGCHLD, sig_child);


	// Create a UDP socket
	int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if(udp_fd < 0) {
		fprintf(stderr, "Could not create UDP server socket (%s)", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(bind(udp_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "Error from bind(): %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	server_handler(tcp_fd, udp_fd);
	
	return EXIT_SUCCESS;
}

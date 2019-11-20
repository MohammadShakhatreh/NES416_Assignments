#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>	
#include <arpa/inet.h>
#include <netinet/in.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

#include "generic_server.h"

void sig_child(int signo) {
	
	int stat;
	pid_t pid;
	
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("Child %d terminated.\n", pid);
}

void start_server(struct sockaddr_in *servaddr, void (*handler)(int)) {
	// Create a TCP IPv4 socket
	int sfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sfd < 0) {
		fprintf(stderr, "Could not create server socket (%s)", strerror(errno));
		exit(-1);
	}

	if(bind(sfd, (struct sockaddr *) servaddr, sizeof(*servaddr)) < 0) {
		fprintf(stderr, "Error from bind(): %s", strerror(errno));
		exit(-1);
	}

	if(listen(sfd, LISTENQ) < 0) {
		fprintf(stderr, "Error from listen(): %s", strerror(errno));
		exit(-1);
	}

	// Handle zombie child processes
	signal(SIGCHLD, sig_child);

	pid_t pid;
	char buff[INET_ADDRSTRLEN];
	struct sockaddr_in cliaddr;
	socklen_t sock_len = sizeof(cliaddr);

	puts("listening for connections...");
	while(true) {
		int cfd = accept(sfd, (struct sockaddr *) &cliaddr, &sock_len);
		
		printf("Connection from %s, port %d\n",
			inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
 			ntohs(cliaddr.sin_port)
 		);

		if(cfd < 0) {
			fprintf(stderr, "Error from accept(): (%s)", strerror(errno));
			exit(EXIT_FAILURE);
		}

		switch (fork()) {
			case -1:
				fprintf(stderr, "Could not create child (%s)", strerror(errno));
				break;

			case 0: // Child
				close(sfd); // Unneeded copy of listening socket
				(*handler)(cfd);
				exit(EXIT_SUCCESS);
		}
		close(cfd);
	}

	// Close the main socket
	close(sfd);
}

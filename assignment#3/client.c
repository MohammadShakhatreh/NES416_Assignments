#include <sys/socket.h>
#include <netinet/in.h>	
#include <arpa/inet.h>
#include <netinet/in.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>

#define	MAXLINE	8192

void sig_handler(int signo) {
	puts("Good bye");

	// Terminate the client
	exit(0);
}

void str_cli(int sfd) {
	
	int n;	
	char request[MAXLINE], response[MAXLINE];
	
	// First read the help msg from server
	n = read(sfd, response, MAXLINE);
	printf("%s\n", response);

	printf("> ");
	while (fgets(request, MAXLINE, stdin) != NULL) {
		n = write(sfd, request, strlen(request));
		if (n < 0) perror("str_cli: write error"); 
		
		
		n = read(sfd, response, MAXLINE);
		if (n < 0) perror("str_cli: read error");
		response[n] = 0;

		printf("%s\n", response);
		printf("> ");
	}
}

int main(int argc, char ** argv) {

	if(argc != 3){
		printf("usage: %s ip_addr port", argv[0]);
		return EXIT_FAILURE;
	}

	// Handle signal
	signal(SIGQUIT, sig_handler);

	int sfd;
	struct sockaddr_in servaddr;
	
	sfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sfd < 0) {
		fprintf(stderr, "Could not create a socket (%s)", strerror(errno));
		return EXIT_FAILURE;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	// Convert an ip addr to binary
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0) {
		fprintf(stderr, "%s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	servaddr.sin_port = htons(atoi(argv[2]));

	if (connect(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "Error from connect(): (%s)", strerror(errno));
	}
	
	str_cli(sfd);
	
	return 0;
}
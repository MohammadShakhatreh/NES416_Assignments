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
#include <ctype.h>

#define	MAXLINE	8192

void arrays_cli(int sfd) {
	
	int n;	
	char request[MAXLINE], response[MAXLINE];

	puts("The format for the request is as follows:");
	puts("First specify the type of the request (M for Merge and R for Reverse)");
	puts("then the first array len and the elements separated by a space");
	puts("If the request is merge then specify the second array len and the elements");
	puts("Example request is: M 3 10 20 30 2 40 50");
	puts("This request is Merge with two arrays the first is of length 3");
	puts("and the second is of length 2");
	while (fgets(request, MAXLINE, stdin) != NULL) {
		n = write(sfd, request, strlen(request));
		if (n < 0) perror("str_cli: write error"); 
		
		
		n = read(sfd, response, MAXLINE);
		if (n < 0) perror("str_cli: read error");
		response[n] = '\0';

		printf("%s\n", response);
	}
}

int main(int argc, char ** argv) {

	if(argc != 3){
		printf("usage: %s ip_addr port", argv[0]);
		return EXIT_FAILURE;
	}

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
	
	arrays_cli(sfd);
	
	return 0;
}
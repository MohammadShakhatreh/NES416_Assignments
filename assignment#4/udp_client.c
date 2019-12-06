#include <netdb.h>
#include <sys/socket.h>
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

#define MAXLINE 8192

int main(int argc, char const *argv[]) {

	if(argc != 3){
		printf("usage: %s host port", argv[0]);
		exit(EXIT_FAILURE);
	}

	int sfd;
	struct sockaddr_in servaddr;
	
	if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "Could not create a UDP socket (%s)", strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	struct hostent * host_ptr;
	
	host_ptr = gethostbyname(argv[1]);
	if(host_ptr == NULL) {
		printf("Cannot Resolve Name. Error is: %s.\n", hstrerror(h_errno));
		exit(EXIT_FAILURE);
	}

	// Convert an ip addr to binary
	if(inet_pton(AF_INET, host_ptr->h_addr_list[0], &servaddr.sin_addr) < 0) {
		fprintf(stderr, "%s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	servaddr.sin_port = htons(atoi(argv[2]));

	// To receive async errors
	if (connect(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "Error from connect(): (%s)", strerror(errno));
		exit(EXIT_FAILURE);
	}

	int sz;
	char buff[MAXLINE];

	while(fgets(buff, MAXLINE, stdin) != NULL) {

		sz = write(sfd, buff, strlen(buff));
		sz = read(sfd, buff, MAXLINE);

		buff[sz] = '\0';
		fputs(buff, stdout);
	}

	return 0;
}
#include <stdio.h>

#define MAXLINE 1024

void handler(FILE* fp, int sfd) {
	int maxfdp1;
	fd_set rset;
	char request[MAXLINE], response[MAXLINE];

	FD_ZERO(&rset);
	while(true) {
		FD_SET(fileno(fp), &rset);
		FD_SET(sfd, &rset);
		maxfdp1 = max(fileno(fp), sfd) + 1;

		select(maxfdp1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sfd, &rset)) {
			// Error reading from server
			if(read(sfd, response, MAXLINE) < 0) {
				fprintf(stderr, "Error from read(): (%s)", strerror(errno));
				exit(EXIT_FAILURE);
			}

			fputs(response, stdout);
		}

		if(FD_ISSET(fileno(fp), &rset)) {
			// Client done typing EOF
			if(fgets(request, MAXLINE, fp) == NULL)
				return;

			write(sfd, request, strlen(request));
		}
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
	
	handler(stdin, sfd);
	
	return 0;
}
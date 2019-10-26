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

#define	MAXLINE	1024

void get_html_page(int sfd, char *res) {
	
	int n;	
	char response[MAXLINE];

	char request[] = "\
GET /currency-list.html HTTP/1.1\r\n\
Host: www.exchange-rate.com\r\n\r\n";
	
	// Send the request
	write(sfd, request, MAXLINE);

	while((n = read(sfd, response, MAXLINE - 1)) != 0)
		strcat(res, response);

}

void get_currency(const char *country, const char *page) {

	// last index of the country in page
	int p = -1;

	for(int i = 0; page[i] != '\0'; ++i) {

		int found = true;
		for(int j = 0, k = i; country[j] != '\0'; ++j, ++k) {
			if(country[j] != page[k]) {
				found = false;
			} else {
				p = k;
			}
		}

		if(found) break;
		else p = -1;
	}

	if(p == -1){
		puts("The country is not found!");
		return;
	}


	// Walking to the currency
	int curr_idx = p;
	for(int i = p + 1; page[i] != '\0'; ++i) {
		if(strncmp("<div>", page + i, 5) == 0) {
			curr_idx = i+5;
			break;
		}
	}

	int idx = 0;
	char curr[100];
	for(idx = 0; page[curr_idx] != '<'; ++idx, ++curr_idx)
		curr[idx] = page[curr_idx];
	curr[idx] = '\0';

	printf("The currency of %s is %s\n", country, curr);
}

int main(int argc, char ** argv) {

	if(argc != 2){
		printf("usage: %s country", argv[0]);
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
	if(inet_pton(AF_INET, "66.43.59.201", &servaddr.sin_addr) < 0) {
		fprintf(stderr, "%s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	servaddr.sin_port = htons(80);

	if (connect(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "Error from connect(): (%s)", strerror(errno));
	}
	
	char page[1 << 18];
	get_html_page(sfd, page);

	// Close the connection no need for it now
	close(sfd);

	char *country = argv[1];

	get_currency(country, page);
	
	return 0;
}
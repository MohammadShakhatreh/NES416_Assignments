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

#include "generic_server.h"

#define	LISTENQ	1024
#define	MAXLINE	8192

const char *help = "\
1 - Change the string to capital letters\n\
2 - Count number of words in the string\n\
3 - Count the frequency of some character\n\
4 - Is the string statement or question\n\
5 - help\n\
6 - Exit\n";

void upper_case(int cfd, const char *str) {
	if(strlen(str) == 0){
		write(cfd, "1 No string is provided.", 25);
		return;
	}

	printf("upper_case: %s", str);

	char res[MAXLINE];
	strcpy(res, str);

	for(int i = 0; res[i] != '\0'; ++i)
		res[i] = toupper(res[i]);

	write(cfd, "0 ", 3);
	write(cfd, res, strlen(res));
}

void count_words(int cfd, const char *str){
	if(strlen(str) == 0){
		write(cfd, "1 No string is provided.", 25);
		return;
	}

	printf("count_words: %s", str);


	int spaces = 0;
	for(int i = 0; str[i] != '\0'; ++i) {
		if(str[i] == ' ')
			++spaces;
	}

	char res[30];
	// Convert int to str
	sprintf(res, "%d", spaces + 1);
	// Number of words is spaces + 1
	write(cfd, "0 ", 3);
	write(cfd, res, strlen(res));
}

void count_char_freq(int cfd, char arg, const char *str) {
	if(strlen(str) == 0){
		write(cfd, "1 No string is provided.", 25);
		return;
	}


	int cnt = 0;
	for (int i = 0; str[i] != '\0'; ++i){
		if(str[i] == arg) ++cnt;
	}

	char res[30];
	// Convert int to str
	sprintf(res, "%d", cnt);
	if(cnt){
		write(cfd, "0 ", 3);
		write(cfd, res, strlen(res));
	} else {
		write(cfd, "1 There is no occurance of this character in the string.", 57);
	}
}

void question_or_statment(int cfd, const char *str) {
	if(strlen(str) == 0){
		write(cfd, "1 No string is provided.", 25);
		return;
	}

	int last = strlen(str) - 2;

	if(str[last] == '?')
		write(cfd, "0 Question.", 12);
	else
		write(cfd, "0 Statement.", 13);
}

void execute(int cfd, const char *buf) {
	if(strlen(buf) == 1)
		return;

	printf("the request is: %s", buf);
	
	int type;
	char arg, str[MAXLINE];

	sscanf(buf, "%d", &type);
	switch(type) {
		case 1:
			strcpy(str, buf+2);
			upper_case(cfd, str);
			break;
		case 2:
			strcpy(str, buf+2);
			count_words(cfd, str);
			break;
		case 3:
			arg = buf[2];
			strcpy(str, buf+4);
			count_char_freq(cfd, arg, str);
			break;
		case 4:
			strcpy(str, buf+2);
			question_or_statment(cfd, str);
			break;
		case 5:
			write(cfd, help, strlen(help));
			break;
		case 6:
			close(cfd);
			exit(EXIT_SUCCESS);
		default:
			write(cfd, "Invalid request type.", 22);
	}
}

void request_handler(int cfd) {

	char buf[MAXLINE];
	ssize_t read_chars;

	write(cfd, help, strlen(help));
	write(cfd, "> ", 3);
	while ((read_chars = read(cfd, buf, MAXLINE)) > 0) {
		buf[read_chars] = '\0'; // Add NULL to the end of the string

		execute(cfd, buf);
		//printf("");
		write(cfd, "\n> ", 3);
	}

	if (read_chars < 0) {
		fprintf(stderr, "Error from read(): (%s)", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

// testing code
int main(int argc, char *argv[]) {
	if(argc != 3) {
		printf("usage: %s ip_addr port", argv[0]);
		return EXIT_FAILURE;
	}


	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));

	// IPv4
	servaddr.sin_family = AF_INET;

	// Convert an ip addr to binary
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0){
		fprintf(stderr, "%s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	// Port number
	servaddr.sin_port = htons(atoi(argv[2]));

	// start server 
	start_server(&servaddr, request_handler);
	return EXIT_SUCCESS;
}

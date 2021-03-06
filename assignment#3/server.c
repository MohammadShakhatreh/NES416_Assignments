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
5 - help";

void upper_case(int cfd, const char *str) {
	if(strlen(str) == 0){
		write(cfd, "1 No string is provided.", 26);
		return;
	}

	char tmp[MAXLINE], res[MAXLINE];
	strcpy(tmp, str);

	for(int i = 0; tmp[i] != '\0'; ++i)
		if(isalpha(tmp[i]))
			tmp[i] = toupper(tmp[i]);

	// remove new line char from the string.
	tmp[strlen(tmp) - 1] = '\0';

	sprintf(res, "0 %s", tmp);
	write(cfd, res, strlen(res));
}

void count_words(int cfd, const char *str){
	if(strlen(str) == 0){
		write(cfd, "1 No string is provided.", 26);
		return;
	}

	int spaces = 0;
	for(int i = 0; str[i] != '\0'; ++i) {
		if(str[i] == ' ')
			++spaces;
	}

	char res[30];
	// Convert int to str
	// Number of words is spaces + 1
	sprintf(res, "0 %d", spaces + 1);
	write(cfd, res, strlen(res));
}

void count_char_freq(int cfd, char arg, const char *str) {
	if(strlen(str) == 0){
		write(cfd, "1 No string is provided.", 26);
		return;
	}

	int cnt = 0;
	for (int i = 0; str[i] != '\0'; ++i){
		if(str[i] == arg) ++cnt;
	}

	char res[30];
	// Convert int to str
	sprintf(res, "0 %d", cnt);
	if(cnt){
		write(cfd, res, strlen(res));
	} else {
		write(cfd, "1 There is no occurance of this character in the string.", 58);
	}
}

void question_or_statment(int cfd, const char *str) {
	if(strlen(str) == 0){
		write(cfd, "1 No string is provided.", 26);
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
		default:
			write(cfd, "1 Invalid request type.", 23);
	}
}

void request_handler(int cfd) {

	char buf[MAXLINE];
	ssize_t read_chars;

	write(cfd, help, strlen(help));
	while ((read_chars = read(cfd, buf, MAXLINE)) > 0) {
		buf[read_chars] = '\0'; // Add NULL to the end of the string

		execute(cfd, buf);
	}

	if (read_chars < 0) {
		fprintf(stderr, "Error from read(): (%s)", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

// testing code
int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("usage: %s ip_addr port", argv[0]);
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

	// start server 
	start_server(&servaddr, request_handler);
	return EXIT_SUCCESS;
}

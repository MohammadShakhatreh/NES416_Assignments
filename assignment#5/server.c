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
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>

#define	LISTENQ	1024
#define	MAXLINE	8192
#define MAX_CLIENTS 4

int clients_cnt = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


// Handle zombie child processes
void sig_child(int signo) {
	
	int stat;
	pid_t pid;
	
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0);
}

// Return the number of digits 
// in a number
int digit_cnt(int x) {

	int cnt = 0;
	while(x > 0) {
		x /= 10;
		++cnt;
	}

	return cnt;
}

void merge_command(int cfd, char *str) {

	int a_len;
	
	// Get the length of the first array
	sscanf(str, "%d", &a_len);

	// Go to the first element in the array
	str += digit_cnt(a_len) + 1;

	// Get the elemnts of the first array
	int *a = malloc(a_len * sizeof(int));
	for(int i = 0; i < a_len; ++i) {
		sscanf(str, "%d", a + i);
		str += digit_cnt(a[i]) + 1;
	}

	// The pointer is already on the length
	// of the second array
	
	int b_len;
	// Get the length of the second array
	sscanf(str, "%d", &b_len);

	// Go to the first element of the array
	str += digit_cnt(b_len) + 1;

	// Get the elemnts of the second array
	int *b = malloc(b_len * sizeof(int));
	for(int i = 0; i < b_len; ++i) {
		sscanf(str, "%d", b + i);
		str += digit_cnt(b[i]) + 1;
	}

	char response[MAXLINE];
	char *res_ptr = response;

	for(int i = 0; i < a_len; ++i) {
		// Handle white spaces between elements
		// And to put a space at the end of the string
		if(i != 0) {
			*res_ptr = ' ';
			res_ptr += 1;
		}

		sprintf(res_ptr, "%d", a[i]);
	    res_ptr += digit_cnt(a[i]);
	}

	// Space between the two arrays
	*res_ptr = ' ';
	res_ptr += 1;

	for(int i = 0; i < b_len; ++i) {
		if(i != 0) {
			*res_ptr = ' ';
			res_ptr += 1;
		}

		sprintf(res_ptr, "%d", b[i]);
	    res_ptr += digit_cnt(b[i]);
	}


	// Write the response
	write(cfd, response, strlen(response));
}

void reverse_command(int cfd, char *str) {

	int len;
	
	// Get the length of the first array
	sscanf(str, "%d", &len);

	// Go to the first element in the array
	str += digit_cnt(len) + 1;

	// Get the elements of the array
	int *x = malloc(len * sizeof(int));
	for(int i = 0; i < len; ++i) {
		sscanf(str, "%d", x + i);
		str += digit_cnt(x[i]) + 1;
	}

	// The reverse operation
	for(int i = 0, tmp; i < (len / 2); ++i) {
		tmp = x[i];
	    x[i] = x[len - i - 1];
	    x[len - i - 1] = tmp;
  	}


	char response[MAXLINE];

	// Pointer to walk through the response string
	char *res_ptr = response;

	for(int i = 0; i < len; ++i) {
		// Handle white spaces between elements
		// And to put a space at the end of the string
		if(i != 0) {
			*res_ptr = ' ';
			res_ptr += 1;
		}

		sprintf(res_ptr, "%d", x[i]);
	    res_ptr += digit_cnt(x[i]);
	}

	// Write the response
	write(cfd, response, strlen(response));
}

void request_handler(int cfd) {

	char buf[MAXLINE];
	ssize_t read_chars;
	int request_cnt = 1;

	while ((read_chars = read(cfd, buf, MAXLINE)) > 0) {
		buf[read_chars] = '\0'; // Add NULL to the end of the string

		printf("Request#%d\n", request_cnt++);

		// Pointer to move in the buffer
		char *ptr = buf;

		char op = *ptr;
		if(op == 'M' || op == 'm') {

			ptr += 2;
			merge_command(cfd, ptr);

		} else if(op == 'R' || op == 'r') {

			ptr += 2;
			reverse_command(cfd, ptr);

		} else {
			write(cfd, "Operation not found", 19);
		}
	}

	if (read_chars < 0) {
		fprintf(stderr, "Error from read(): (%s)", strerror(errno));
		exit(EXIT_FAILURE);
	}
}


void *run(void *arg) {
	int cfd = *((int *) arg);
	free(arg);
	
	pthread_detach(pthread_self());
	request_handler(cfd);
	close(cfd);

	int ret;
	ret = pthread_mutex_lock(&mutex);
	if(ret != 0) perror("Error locking a mutex.\n");

	--clients_cnt;
	pthread_cond_signal(&cond);

	ret = pthread_mutex_unlock(&mutex);
	if(ret != 0) perror("Error unlocking a mutex.\n");

	return NULL;
}


int main(int argc, char *argv[]) {
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
	int sfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sfd < 0) {
		fprintf(stderr, "Could not create TCP server socket (%s)", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(bind(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "Error from bind(): %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(listen(sfd, LISTENQ) < 0) {
		fprintf(stderr, "Error from listen(): %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Handle zombie child processes
	signal(SIGCHLD, sig_child);

	int ret;
	int *cfd;
	pthread_t tid;

	puts("listening for connections...");
	while(true) {
		cfd = malloc(sizeof(int));
		*cfd = accept(sfd, NULL, NULL);

		if(*cfd < 0) {
			fprintf(stderr, "Error from accept(): (%s)", strerror(errno));
			exit(EXIT_FAILURE);
		}

		pthread_create(&tid, NULL, &run, cfd);
			
		ret = pthread_mutex_lock(&mutex);
		if(ret != 0) perror("Error locking a mutex.\n");

		++clients_cnt;
		while(clients_cnt == MAX_CLIENTS) pthread_cond_wait(&cond, &mutex);

		ret = pthread_mutex_unlock(&mutex);
		if(ret != 0) perror("Error unlocking a mutex.\n");
	}

	return EXIT_SUCCESS;
}

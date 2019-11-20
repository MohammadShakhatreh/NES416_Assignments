#ifndef _GENERIC_SERVER_H
#define _GENERIC_SERVER_H

#include <netinet/in.h>

#define	LISTENQ	1024
#define	MAXLINE	8192


/* 
* This function is used to setup the server
* and handle the requests using the provided handler function
*/
extern void start_server(struct sockaddr_in *, void (*handler)(int));

#endif
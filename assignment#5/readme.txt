Server
- compile with:
gcc -o server server.c -lpthread

- run with:
./server port

- running sample:

listening for connections...
Request#1
Request#2

-------------------------------
Client

- compile with:
gcc client.c -o client

- run with:
./client ip_addr port

- running sample:

The format for the request is as follows:
First specify the type of the request (M for Merge and R for Reverse)
then the first array len and the elements separated by a space
If the request is merge then specify the second array len and the elements
Example request is: M 3 10 20 30 2 40 50
This request is Merge with two arrays the first is of length 3
and the second is of length 2
M 3 12 13 14 1 15
12 13 14 15 
R 5 1 2 3 4 5
5 4 3 2 1 
^C


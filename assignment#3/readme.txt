Server
- compile with:
gcc server.c generic_server.c -o server

- run with:
./server port

- running sample:

listening for connections...
Connection from 192.168.8.2, port 48618
Child 7420 terminated.
Connection from 127.0.0.1, port 56236
Child 7442 terminated.
^C

-------------------------------
Client

- compile with:
gcc client.c -o client

- run with:
./client ip_addr port

- running sample:

1 - Change the string to capital letters
2 - Count number of words in the string
3 - Count the frequency of some character
4 - Is the string statement or question
5 - help
> 1 foo
0 FOO
> 1
1 No string is provided.
> 4 foo
0 Statement.
> 4 foo?
0 Question.
> 2 one two three
0 3
> 3 u true uni
0 2
> ^\Good bye


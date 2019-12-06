- Compilation for the three file:
gcc filename.c -o filename

- Running samples:

-- Server:
$ ./server 1234
Received UDP request from client: 127.0.0.1, port 57527
Received UDP request from client: 127.0.0.1, port 57527
Received TCP request from client: 127.0.0.1, port 45222

-- UDP Client:
$ ./udp_client localhost 1234
hello
HELLO
there
THERE

-- TCP Client:
$ ./tcp_client localhost 1234
yay!!
YAY!!
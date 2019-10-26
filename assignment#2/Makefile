SRC = generic_server.c first_part.c
OBJ = $(SRC:.c=.o)

server: $(OBJ)
	cc -o $@ $(OBJ)

generic_server.o: generic_server.h generic_server.c
first_part.o: first_part.c

clean:
	rm server $(OBJ)
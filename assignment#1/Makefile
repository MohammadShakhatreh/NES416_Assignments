SRC = main.c
OBJ = $(SRC:.c=.o)

st: $(OBJ)
	cc -o $@ $(OBJ)

main.o: main.c

clean:
	rm st $(OBJ)
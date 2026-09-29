CFLAGS = -Wall -Wextra -Wno-unused-parameter -Werror -g -fsanitize=address

all : run

vector.o : src/vector.c src/vector.h
	gcc src/vector.c -c $(CFLAGS) -o vector.o

main.o : src/main.c src/vector.h
	gcc src/main.c -c $(CFLAGS) -o main.o

main.exe : main.o vector.o 
	gcc main.o vector.o -o main.exe $(CFLAGS) -lm

run : main.exe
	./main.exe

clean:
	rm -f *.o *.exe

.PHONY: clean all run
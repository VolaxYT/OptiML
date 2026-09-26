CFLAGS = -Wall -Wextra -Wno-unused-parameter -Werror -g -fsanitize=address
BENCH_CFLAGS = -O3 -march=native -Wall -Wextra 

all : run

# --- build debug ---

vector.o : src/vector.c src/vector.h
	gcc src/vector.c -c $(CFLAGS) -o vector.o

main.o : src/main.c src/vector.h
	gcc src/main.c -c $(CFLAGS) -o main.o

main.exe : main.o vector.o 
	gcc main.o vector.o -o main.exe $(CFLAGS) -lm

run : main.exe
	./main.exe

# --- build benchmark ---

vector_bench.o : src/vector.c src/vector.h
	gcc src/vector.c -c $(BENCH_CFLAGS) -o vector_bench.o

benchmark.exe : src/benchmark/benchmark.c vector_bench.o
	gcc src/benchmark/benchmark.c vector_bench.o $(BENCH_CFLAGS) -o benchmark.exe -lm

bench: benchmark.exe
	./benchmark.exe

clean:
	rm -f *.o *.exe

.PHONY: clean all run
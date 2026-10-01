CFLAGS = -Wall -Wextra -Wno-unused-parameter -Werror -g -fsanitize=address -mavx2 -mfma
BENCH_CFLAGS = -O3 -mavx2 -mfma -Wall -Wextra 

all : run

# --- build debug ---

memory.o : src/memory.c src/memory.h
	gcc src/memory.c -c $(CFLAGS) -o memory.o

matrix.o : src/matrix.c src/matrix.h src/memory.h
	gcc src/matrix.c -c $(CFLAGS) -o matrix.o

vector.o : src/vector.c src/vector.h src/memory.h
	gcc src/vector.c -c $(CFLAGS) -o vector.o

main.o : src/main.c src/vector.h src/matrix.h src/memory.h
	gcc src/main.c -c $(CFLAGS) -o main.o

main.exe : main.o vector.o matrix.o memory.o
	gcc main.o vector.o matrix.o memory.o -o main.exe $(CFLAGS) -lm

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
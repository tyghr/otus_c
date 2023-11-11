CFLAGS = -g -Wall -Wextra -Wpedantic -std=c11

build:
	gcc ${CFLAGS} -c -o main.o main.c
	gcc ${CFLAGS} -o appd main.o -lconfuse

clean:
	rm app appd a.out main.o 2>/dev/null || true

CFLAGS = -g -Wall -Wextra -Wpedantic -std=c11

build:
	gcc ${CFLAGS} -c -o main.o main.c
	gcc ${CFLAGS} -o app main.o -lglut -lGL

clean:
	rm app main.o a.out 2>/dev/null || true

CFLAGS = -g -Wall -Wextra -Wpedantic -std=c11

build:
	gcc ${CFLAGS} -c -o main.o main.c
	gcc ${CFLAGS} -o app main.o -lcurl -ljson-c

clean:
	rm main.o a.out app

CFLAGS = -g -Wall -Wextra -Wpedantic -std=c11

build:
	gcc ${CFLAGS} -c -o main.o main.c
	gcc ${CFLAGS} -c -o hashmap.o hashmap.c
	gcc ${CFLAGS} -o app main.o hashmap.o

clean:
	rm main.o hashmap.o a.out app

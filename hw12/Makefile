CFLAGS = -g -Wall -Wextra -Wpedantic -std=c11

build: lib
	gcc ${CFLAGS} -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -c -o main.o main.c
	gcc ${CFLAGS} -o app main.o -lglib-2.0 -L. -lchan

lib:
	gcc ${CFLAGS} -c -o chan.o ./chan/chan.c
	gcc ${CFLAGS} -c -o queue.o ./chan/queue.c
	ar cr libchan.a chan.o queue.o

clean:
	rm app libchan.a chan.o queue.o main.o a.out 2>/dev/null || true

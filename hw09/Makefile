CFLAGS = -g -Wall -Wextra -Wpedantic -std=c11

build: lib
	gcc ${CFLAGS} -c -o main.o main.c
	gcc ${CFLAGS} -o app main.o -L. -lclogger_static

lib:
	gcc ${CFLAGS} -c -o logger.o ./libclogger/logger.c
	gcc ${CFLAGS} -c -o loggerconf.o ./libclogger/loggerconf.c
	ar cr libclogger_static.a logger.o loggerconf.o

clean:
	rm app libclogger_static.a logger.o loggerconf.o main.o a.out 2>/dev/null || true

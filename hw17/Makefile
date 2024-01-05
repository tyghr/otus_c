CFLAGS = -g -Wall -Wextra -Wpedantic -std=c11

build:
	gcc ${CFLAGS} -c -o main.o `sdl2-config --cflags` main.c
	gcc ${CFLAGS} -o app main.o -lm `sdl2-config --libs`

# builddemo:
# 	gcc ${CFLAGS} -c -o demo.o `sdl2-config --cflags` demo.c
# 	gcc ${CFLAGS} -o app demo.o -lm -ldl `sdl2-config --libs`

clean:
	rm app main.o a.out 2>/dev/null || true

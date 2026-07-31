all:
	gcc -c raylib_wrapper.c -o raylib_wrapper.o

	../../wlangc pong.w --compile-only --no-start-fun
	as out/pong.s -o pong.o

	gcc -o pong pong.o raylib_wrapper.o -L../../ -lw -L. -lraylib -lm -ldl -lpthread -lX11 -lGL -lGLU -lglfw

clean:
	rm -f pong pong.o pong.s raylib_wrapper.o
	rm -rf ./out

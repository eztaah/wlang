default:
	../../wlangc library.w --lib

	gcc -o prog main.c out/lib.a -L./ -L../../ -lw

clean:
	rm -f prog
	rm -rf ./out

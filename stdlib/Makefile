SOURCE_FILES = src/*.w 
FLAGS = --lib --verbose --define C_MEMORY --define C_WRITE  # there is no need to add --no-libw because we are creating a .a lib

default:
	../wlangc $(SOURCE_FILES) $(FLAGS)
	mv ./out/lib.a ../libw.a

clean:
	rm -f prog
	rm -f out/*
	rmdir out/
	rm -f ../libw.a

SOURCE_FILES = src/*.w 
FLAGS = --lib --define C_MEMORY  # there is no need to add --no-libw because we are creating a .a lib

default:
	../wlangc $(SOURCE_FILES) $(FLAGS)
	mv ./out/lib.a ../libw.a

verbose: 
	../wlangc $(SOURCE_FILES) $(FLAGS) --verbose
	mv ./out/lib.a ../libw.a

clean:
	rm -f prog
	rm -f out/*
	rmdir out/
	rm -f ../libw.a

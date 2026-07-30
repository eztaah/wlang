SOURCE_FILES = src/*.w 
FLAGS = --lib --verbose --define USE_C_MEMORY_FUN

default:
	../wlangc $(SOURCE_FILES) $(FLAGS)
	mv ./out/lib.a ../libw.a

clean:
	rm -f prog
	rm -f out/*
	rmdir out/
	rm -f ../libw.a

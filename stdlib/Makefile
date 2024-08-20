SOURCE_FILES = io/*.w 
FLAGS = --lib

default:
	../wlangc $(SOURCE_FILES) $(FLAGS)
	mv ./out/lib.a ../libw.a

clean:
	rm -f prog
	rm -f out/*
	rmdir out/
	rm ../libw.a

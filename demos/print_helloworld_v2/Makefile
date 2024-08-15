SOURCE_FILES = ./main.w ./std/io/print.w ./std/memory/memory.w
FLAGS = --dev-mode --to-executable --verbose

default:
	../../compiler/wlangc $(SOURCE_FILES) $(FLAGS)

clean:
	rm -f prog
	rm -f out/*
	rmdir out/

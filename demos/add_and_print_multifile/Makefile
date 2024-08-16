SOURCE_FILES = ./main.w ./add.w ./io.w
FLAGS = --dev-mode --to-executable --verbose

default:
	../../compiler/wlangc $(SOURCE_FILES) $(FLAGS)

clean:
	rm -f prog
	rm -f out/*
	rmdir out/

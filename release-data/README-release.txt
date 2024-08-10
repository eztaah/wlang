This directory contains the following:

- wlangc: The wlang compiler.
- libw.a: The standard library for wlang.
- hello-world: A folder containing an example wlang program.
- CHANGELOG.txt: A file listing the changes in this version.
- README.txt: This file.


Important Information:

By default, wlangc automatically links your code with the wlang standard library (libw.a) (unless the --no-libw flag is specified). The compiler expects the libw.a file to be in the same directory as wlangc. Therefore, ensure that both ./wlangc and ./libw.a are always in the same folder.

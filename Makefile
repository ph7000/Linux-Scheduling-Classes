fibonacci:

oven_shell: oven_shell.c
	gcc -Wall oven_shell.c -o oven_shell
all: fibonacci oven_shell
.PHONY: clean
clean:
	rm fibonacci oven_shell
dragonshell: dragonshell.o
	gcc -Wall -o dragonshell dragonshell.o

compile: dragonshell.c
	gcc -Wall -c dragonshell.c

clean:
	rm -f dragonshell.o dragonshell

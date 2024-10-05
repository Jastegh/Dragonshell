dragonshell: dragonshell.o utils.o
	gcc -Wall - -g -o dragonshell dragonshell.o utils.o

compile: dragonshell.o utils.o

dragonshell.o: dragonshell.c
	# Compile dragonshell.c into dragonshell.o
	gcc -Wall -Wextra -g -c dragonshell.c


clean:
	rm -f dragonshell.o  dragonshell
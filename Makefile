CC = gcc

rpg:
	$(CC) -march=native -O2 -o rpg rpg.c

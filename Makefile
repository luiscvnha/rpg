CC = gcc
CFLAGS = -O2

rpg: rpg.c
	$(CC) $(CFLAGS) -o rpg rpg.c

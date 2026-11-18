CC := gcc
CFLAGS := -Wall -Wextra -O2

BIN := rpg


.PHONY: clean


${BIN}: ${BIN}.c
	${CC} ${CFLAGS} -o $@ $^

clean:
	rm -f ${BIN}

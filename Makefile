CC = gcc
CFLAGS = -std=c11 -Wall -Werror

all: mysync

mysync: mysync.c glob2regex.c
    $(CC) $(CFLAGS) -o mysync mysync.c glob2regex.c

clean:
    rm -f mysync

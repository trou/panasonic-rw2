CC=gcc -W -Wall -Werror -std=c90
all:
	$(CC) -o parse parse.c
	$(CC) -o dng parsedng.c

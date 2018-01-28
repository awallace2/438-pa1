CC=g++

all: csrd crc

csrd: csrd.c
	$(CC) -o csrd.out csrd.c

crc: crc.c
	$(CC) -o crc.out crc.c

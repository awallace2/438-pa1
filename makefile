CC=g++

csrd: csrd.c
	$(CC) -o csrd.out csrd.c

crc: crc.c
	$(CC) -o crc.out crc.c

all: csrd crc


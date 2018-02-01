CC=g++

HOST=http://localhost/
PORT=8888

all: csrd crc

csrd: csrd.c
	$(CC) -std=c++11 -pthread -g -o csrd.out csrd.c

crc: crc.c
	$(CC) -std=c++11 -pthread -g -o crc.out crc.c

runc:
	# Run server in background and then client in foreground after 2 sec
	./crc.out $(HOST) $(PORT)

runs:
	./csrd.out $(HOST) $(PORT)

clean:
	rm *.out
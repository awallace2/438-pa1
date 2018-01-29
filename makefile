CC=g++

HOST=http://localhost/
PORT=6969

all: csrd crc

csrd: csrd.c
	$(CC) -o csrd.out csrd.c

crc: crc.c
	$(CC) -o crc.out crc.c

run:
	# Run server in background and then client in foreground after 2 sec
	./csrd.out $(HOST) $(PORT) & sleep 2 && ./crc.out $(HOST) $(PORT)

clean:
	rm *.out
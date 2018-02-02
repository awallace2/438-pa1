CC=g++ -pthread -std=c++11

HOST=http://localhost/
PORT=6931

all: csrd crc

debug: csrd_d crc_d

csrd: csrd.c
	$(CC) -o csrd.out csrd.c

csrd_d: csrd.c
	$(CC) -o csrd.out csrd.c -g

crc: crc.c
	$(CC) -o crc.out crc.c

crc_d: crc.c
	$(CC) -o crc.out crc.c -g

run:
	# Run server in background and then client in foreground after 2 sec
	./csrd.out $(HOST) $(PORT) & sleep 2 && ./crc.out $(HOST) $(PORT)

clean:
	rm *.out

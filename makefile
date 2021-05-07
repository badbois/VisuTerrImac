CC=g++
CFLAGS=-Wall -ansi



test: main.o read.o
	$(CC) -o $@ $^
	

main.o: main.cpp read.h
	$(CC) -o $@ -c $< $(CFLAGS)

read.o: read.cpp read.h
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o
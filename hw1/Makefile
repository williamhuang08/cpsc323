CC = gcc
CFLAGS = -g3 -Wall -std=c11 -pedantic

proj1: proj1.o
	$(CC) $(CFLAGS) -o $@ $^

proj1.o: proj1.c proj1.h

clean:
	rm -f *.o proj1

compare: proj1
	./proj1 $(file) > my_output.txt
	/c/cs323/proj1/proj1 $(file) > expected_output.txt
	diff my_output.txt expected_output.txt
	
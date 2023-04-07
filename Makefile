main: *.c *.h
	gcc -Wall -Werror -Ofast -o main *.c

run: main
	./main

clean:
	rm -f main

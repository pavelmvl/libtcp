
.PHONY: run
run: tcp
	./tcp

tcp: main.c $(wildcard ../src/*.c)
	gcc -g -fPIC -I../include -pthread -o tcp $^

.PHONY: clean
clean:
	${RM} tcp

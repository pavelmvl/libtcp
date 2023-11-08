
.PHONY: run
run: tcp
	./tcp

tcp: main.c ../src/print_sockaddr_in.c ../src/tcpServer.c
	gcc -I../include -lpthread -o tcp $^

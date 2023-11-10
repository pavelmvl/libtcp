
.PHONY: run
run: tcp
	./tcp

tcp: main.c ../src/print_sockaddr_in.c ../src/tcpServer.c ../src/tcpServer_UpdateBuffer.c
	gcc -I../include -lpthread -o tcp $^

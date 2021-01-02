.PHONY: default dist clean

default: client server

client: client.cpp
	g++-8 -std=c++17  client.cpp -o client
	
server: server.cpp
	g++-8 -std=c++17 server.cpp -o server

dist: client.cpp server.cpp Makefile README report.pdf
	tar -czf project2_404465866_704766877.tar client.cpp server.cpp Makefile README report.pdf

clean:
	rm -rf project2_404465866_704766877.tar client server

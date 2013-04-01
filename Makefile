FLAGS=-Wall -g -pedantic
# DFLAGS=-Wall -g -pedantic
# RFLAGS=-O3
# OBJS = table.o logger.o socket.o serversocket.o clientsocket.o parser.o request.o
# UTIL_HEADERS = threadutils.h hashutils.h shmutils.h serverutils.h serverutils.h

# set to debug flags
debug: all

# set to release flags
release: all

all: master_server worker_server client

master_server: table.o logger.o socket.o serversocket.o clientsocket.o parser.o request.o master_main.cpp threadutils.h hashutils.h shmutils.h serverutils.h
	g++ $(FLAGS) master_main.cpp serversocket.o clientsocket.o socket.o request.o table.o -o master_server -lrt

worker_server: table.o logger.o socket.o serversocket.o clientsocket.o parser.o request.o worker_main.cpp threadutils.h hashutils.h shmutils.h serverutils.h
	g++ $(FLAGS) worker_main.cpp serversocket.o clientsocket.o socket.o request.o table.o -o worker_server -lrt

client: table.o logger.o socket.o clientsocket.o parser.o request.o client_main.cpp threadutils.h hashutils.h shmutils.h serverutils.h
	g++ $(FLAGS) client_main.cpp clientsocket.o socket.o request.o table.o parser.o -o client -lrt

table.o: table.h table.cpp
	g++ $(FLAGS) -c table.cpp

logger.o: logger.h logger.cpp
	g++ $(FLAGS) -c logger.cpp

serversocket.o: serversocket.h serversocket.cpp socket.o
	g++ $(FLAGS) -c serversocket.cpp

clientsocket.o: clientsocket.h clientsocket.cpp socket.o
	g++ $(FLAGS) -c clientsocket.cpp

socket.o: socket.h socket.cpp
	g++ $(FLAGS) -c socket.cpp

parser.o: parser.h parser.cpp
	g++ $(FLAGS) -c parser.cpp

request.o: request.h request.cpp
	g++ $(FLAGS) -c request.cpp

clean:
	rm -f table.o logger.o socket.o serversocket.o clientsocket.o parser.o request.o master_server worker_server client /dev/shm/sem* /dev/shm/shm_table*

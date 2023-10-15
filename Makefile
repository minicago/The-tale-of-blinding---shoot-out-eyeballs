libcpp := Message.cpp Socket.cpp Game.cpp
libh := Message.h Socket.h Game.h
all : Server.out Client.out


Server.out : Server.cpp $(libcpp) $(libh)
	g++ $(libcpp) Server.cpp -o Server.out -lpthread

Client.out : Client.cpp $(libcpp) $(libh)
	g++ -DRELEASE $(libcpp) Client.cpp -o Client.out -lpthread

.PHONY: clean
clean : 
	rm *.out
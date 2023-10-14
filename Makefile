libcpp := Message.cpp Socket.cpp Game.cpp
libh := Message.h Socket.h Game.h
all : Server.exe Client.exe

Server.exe : Server.cpp $(libcpp) $(libh)
	g++ $(libcpp) Server.cpp -o Server.exe -lwsock32
Client.exe : Client.cpp $(libcpp) $(libh)
	g++ -DRELEASE $(libcpp) Client.cpp -o Client.exe -lwsock32

.PHONY: clean
clean : 
	del *.exe
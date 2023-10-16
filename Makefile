libcpp := Message.cpp Socket.cpp Game.cpp UI.cpp
libh := Message.h Socket.h Game.h UI.h
Windows : Server.exe Client.exe

Server.exe : Server.cpp $(libcpp) $(libh)
	g++ $(libcpp) Server.cpp -o Server.exe -lwsock32
Client.exe : Client.cpp $(libcpp) $(libh)
	g++ -static -DRELEASE $(libcpp) Client.cpp -o Client.exe -lwsock32 

Linux : Server.out Client.out
Server.out : Server.cpp $(libcpp) $(libh)
	g++ $(libcpp) Server.cpp -o Server.out -lpthread
Client.out : Client.cpp $(libcpp) $(libh)
	g++ -DRELEASE $(libcpp) Client.cpp -o Client.out -lpthread


.PHONY: clean
clean : 
	del *.exe
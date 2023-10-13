all : Client.exe Server.exe
Server.exe: Server.cpp
	g++ .\Game.cpp .\Server.cpp -o .\Server.exe -lwsock32
Client.exe: Client.cpp
	g++ .\Game.cpp .\Client.cpp -o .\Client.exe -lwsock32
.PHONY: clean
clean : 
	del *.exe
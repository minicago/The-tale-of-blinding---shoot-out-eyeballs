libcpp := Message.cpp Socket.cpp Game.cpp
libh := Message.h Socket.h Game.h
all : Test.exe Server.exe Client.exe

Test.exe : Test.cpp $(libcpp) $(libh)
	g++ $(libcpp) Test.cpp -o Test.exe -lwsock32
Server.exe : Test.cpp $(libcpp) $(libh)
	g++ $(libcpp) Server.cpp -o Server.exe -lwsock32
Client.exe : Client.cpp $(libcpp) $(libh)
	g++ -DRELEASE $(libcpp) Client.cpp -o Client.exe -lwsock32


.PHONY: clean
clean : 
	del *.exe
libcpp := Message.cpp Socket.cpp Game.cpp
libh := Message.h Socket.h Game.h
all : Test.exe

Test.exe : Test.cpp $(libcpp) $(libh)
	g++ $(libcpp) Test.cpp -o Test.exe -lwsock32
.PHONY: clean
clean : 
	del *.exe
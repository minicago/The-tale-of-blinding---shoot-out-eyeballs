#include <stdio.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "Game.h"

#pragma comment(lib,"ws2_32.lib")

const int MAX_Client = 256;

struct Args{
	unsigned int ip;
	unsigned short port;
};

int arg_parse(Args *args,int argc,char* argv[]){
	args->ip = 0;
	args->port = 5050;
	if(argc>3) goto err;
	if(argc>1) args->ip = inet_addr(argv[1]);
	if(args->ip==-1) goto err;
	if(argc>2) args->port = atoi(argv[2]);
	return 0;
err:
	printf("Failed in arg parse!\n");
	return 1;
}

char rootpath[4096];

int Set_addr(sockaddr_in *addr,Args args){
	addr->sin_port = htons(args.port);
	addr->sin_addr.s_addr = args.ip;
	return 0;
}

struct Room{
	SOCKET socket[2];
	Game game;
	int playerNum;
}rooms[MAX_Client];


int main(int argc,char* argv[]){

	srand(time(NULL));

	Args args;
	arg_parse(&args,argc,argv);

	fd_set rfds;				
	fd_set wfds;				
	bool first_connetion = true;


	printf("Winsock  startup Ok!\n");


	SOCKET srvSocket;
	sockaddr_in addr,clientAddr;
	//SOCKET sessionSocket;
	
	socklen_t addrLen;
	//create socket
	srvSocket = socket(AF_INET,SOCK_STREAM,0);
	if(srvSocket != -1)
		printf("Socket create Ok!\n");
	//set port and ip
	addr.sin_family = AF_INET;
	/*addr.sin_port = htons(5050);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);*/

	Set_addr(&addr,args);
	//binding
	
	int rtn = bind(srvSocket, (struct sockaddr*) &addr, sizeof(addr));
	if(rtn != -1)
		printf("Socket bind Ok!\n");
	else {
		printf("Socket bind Error!\n");
		return -1;
	}
	//listen
	rtn = listen(srvSocket,5);
	if(rtn != -1)
		printf("Socket listen Ok!\n");
	else{
		printf("Socket listen Error!\n");
		return -1;		
	}
	clientAddr.sin_family =AF_INET;
	addrLen = sizeof(clientAddr);
	char recvBuf[4096];

	u_long blockMode = 1;

	fcntl(srvSocket, F_SETFL, O_NONBLOCK);


	FD_ZERO(&rfds);
	FD_ZERO(&wfds);

	
	FD_SET(srvSocket, &rfds);

	SOCKET tmp;

	while(true){
		
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);


		FD_SET(srvSocket, &rfds);

		int nTotal = select(1024, &rfds, &wfds, NULL, NULL);

		if (FD_ISSET(srvSocket, &rfds)) {
			nTotal--;
			SOCKET sessionSocket = accept(srvSocket,(sockaddr*) &clientAddr, &addrLen);
			printf("Client ip: %s Client port: %d\n",inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
			if (sessionSocket != -1)
				printf("Socket listen one client request!\n");

				fcntl(sessionSocket, F_SETFL, O_NONBLOCK);

			bool waiting = true;
			for(int i = 0; i <= MAX_Client; i++){
				if(rooms[i].playerNum == 1){
					waiting = false;
					rooms[i].playerNum = 2;
					rooms[i].socket[1] = sessionSocket;
					initGame(&rooms[i].game, 12, rooms[i].socket[0], rooms[i].socket[1]);
					break;
				}
				else if(rooms[i].playerNum == 2){
					if(rooms[i].game.finished) rooms[i].playerNum = 0;
				}
			}
			if(waiting){
				for(int i = 0; i <= MAX_Client; i++){
					if(rooms[i].playerNum == 0){
						waiting = false;
						rooms[i].playerNum = 1;
						rooms[i].socket[0] = sessionSocket;
						break;
					}
				}				
			}
			if(waiting) close(sessionSocket);
			/*if(!waiting){
				waiting = 1;
				tmp = sessionSocket;
			}else{
				initGame(&g, 12, tmp, sessionSocket);
				break;
			}*/
			
			
			//first_connetion = false;
		}
		
		
	}
	return 0;
}
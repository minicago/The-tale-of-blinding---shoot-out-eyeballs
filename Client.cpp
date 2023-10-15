#include <stdio.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include "Game.h"
#include <queue>

#pragma comment(lib,"ws2_32.lib")

using namespace std;
queue<char> movq;

struct Args{
	unsigned int ip;
	unsigned short port;
};

int arg_parse(Args *args,int argc,char* argv[]){
	args->ip = inet_addr("127.0.0.1");
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

int Set_addr(sockaddr_in *addr,Args args){
	//printf("%llu %u\n",args.port,args.ip);
	(addr->sin_port) = htons(args.port);
	
	(addr->sin_addr).s_addr = args.ip;
	return 0;
}

char recvBuf[4096];

Game game;

void UI(){
	while(1){
		printf("Input (1-3)\n");
		printf("1 move\n2 shoot\n3 set bell\n");
		int tmp;
		scanf("%d", &tmp);
		if(tmp == 1){
			game.player[0]->state=p_active;
			printf("Input towards(at most 3 steps) U(p) or D(own) or L(eft) or R(ight) \n");
			printf("Don't split your towards by ',' or ' ' \n");
			char str[128]="MOV ";
			scanf("%s",str+4);
			for(int i=4; str[i]!='\0';i++) movq.push(str[i]);
			str[strlen(str)+1] = '\0';
			str[strlen(str)] = '\n';
			bufInsert(&game.send[0], str);
			break;
		}
		if(tmp == 2){
			if(game.player[0]->state != p_active){
				printf("You need move and then can shoot!\n");
				continue;
			}
			printf("Input towards U(p) or D(own) or L(eft) or R(ight) \n");
			char str[128]="SHOOT ";
			scanf("%s",str+6);
			str[strlen(str)+1] = '\0';
			str[strlen(str)] = '\n';
			bufInsert(&game.send[0], str);
			break;						
		}
		if(tmp == 3){
			if(game.player[0]->equipment != equipBell){
				printf("No bell now!\n");
				continue;
			}
			bufInsert(&game.send[0], "SET 0\n");
			break;
		}
	}



}

int main(int argc,char* argv[]){
	string input;
	Args args;
	pthread_t readthread;
	arg_parse(&args,argc,argv);


	printf("Winsock  startup Ok!\n");


	SOCKET clientSocket;
	sockaddr_in serverAddr,clientAddr;

	int addrLen;

	//create socket
	clientSocket = socket(AF_INET,SOCK_STREAM,0);

	if(clientSocket != -1)
		printf("Socket create Ok!\n");

	//set client port and ip
	clientAddr.sin_family = AF_INET;
	
	clientAddr.sin_port = htons(0);
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	 
	//binding
	int rtn = bind(clientSocket,(sockaddr*)&clientAddr,sizeof(clientAddr));
	if(rtn != -1)
		printf("Socket bind Ok!\n");

	serverAddr.sin_family = AF_INET;
	Set_addr(&serverAddr,args);

	rtn = connect(clientSocket,(sockaddr*)&serverAddr,sizeof(serverAddr));
	if(rtn == -1 )
	{
		printf("Connect to server error!\n");
		return 0;
	}
	
	if(rtn!=0){
		printf("Error in pthread_create!\n");
	}

		printf("Connect to server ok!");

	initClient(&game, clientSocket);

	printf("initClient Ok!\n");

	//bufInsert(&game.send[0],"READY 0\n");

	do {
		DEBUG("*************\n");
		char *message;

        message = pullMessage(&game.recv[0]);

		DEBUG("pull done\n");
		DEBUG("messageadd:%x\n",message);
		DEBUG("message:%s\n",message);
		MessageType msgType = messageParse(message);
        const char* mArg = messageString(message);
		int mInt = messageInt(message);
		switch (msgType){
		case msg_MAPSIZE:
			nullMap(&game, mInt);
			break;
		case msg_POSITIONX:
			game.player[0]->positionX=mInt;
			break;
		case msg_POSITIONY:
			game.player[0]->positionY=mInt;
			break;
		case msg_TEXT:
			mapLog(&game);
			printf("*************\n");
			break;
		case msg_LOSE:
			printf("You lose!\n");
			game.finished = true;
			bufInsert(&game.send[0], "READY 0\n");
			break;
		case msg_WIN:
			printf("You win!\n");
			game.finished = true;
			bufInsert(&game.send[0], "READY 0\n");		
			break;
		case msg_MOV:
			if(mInt == 0){
				char tmp = movq.front();
				if(tmp == 'U'){
					if(game.player[0]->positionX > 0) game.map[game.player[0]->positionX - 1][game.player[0]->positionY] = m_blocked;
				}
				if(tmp == 'D'){
					if(game.player[0]->positionX < game.mapSize - 1) game.map[game.player[0]->positionX + 1][game.player[0]->positionY] = m_blocked;
				}
				if(tmp == 'L'){
					if(game.player[0]->positionY > 0) game.map[game.player[0]->positionX][game.player[0]->positionY - 1] = m_blocked;
				}
				if(tmp == 'R'){
					if(game.player[0]->positionY < game.mapSize - 1) game.map[game.player[0]->positionX][game.player[0]->positionY + 1] = m_blocked;
				}
				while(!movq.empty()) movq.pop();
			} else {
				char tmp = movq.front();
				if(tmp == 'U'){
					game.player[0]->positionX = game.player[0]->positionX - 1; 
				}
				if(tmp == 'D'){
					game.player[0]->positionX = game.player[0]->positionX + 1; 
				}
				if(tmp == 'L'){
					game.player[0]->positionY = game.player[0]->positionY - 1; 
				}
				if(tmp == 'R'){
					game.player[0]->positionY = game.player[0]->positionY + 1; 
				}
				game.map[game.player[0]->positionX][game.player[0]->positionY] = m_empty;
				movq.pop();
			}
			break;
		case msg_VOICE:
			printf("%s", mArg);
			break;
		case msg_READY:
			mapLog(&game);
			printf("*************\n");
			UI();
			printf("*************\n");
			break;
		}
		abandonMessage(&game.recv[0]);
	}while(!game.finished);
	close(clientSocket);
}
//10.12.56.97

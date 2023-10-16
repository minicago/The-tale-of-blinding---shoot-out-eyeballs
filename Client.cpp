#include <stdio.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include "Game.h"
#include <queue>
#include <stdlib.h>

#include "UI.h"

using namespace std;
queue<char> movq;

int arg_parse(Args *args,int argc,char* argv[]){
	args->ip = inet_addr("127.0.0.1");
	args->port = 5050;
	if(argc>3) goto err;
	if(argc>1) args->ip = inet_addr(argv[1]);
	else{
		char ipstr[2048];
		printf("********************\n");
		printf("* Input server IP  *\n");
		printf("* '0'  to default  *\n");
		printf("********************\n");
		scanf("%s",ipstr);
		if(ipstr[0] == '0') return 0;
		args->ip = inet_addr(ipstr);
	}
	if(args->ip==-1) goto err;
	if(argc>2) args->port = atoi(argv[2]);
	else {
		printf("********************\n");
		printf("* Input server Port*\n");
		printf("* '0'  to default  *\n");
		printf("********************\n");
		int port;
		scanf("%d",&port);
		if(port == 0) return 0;
		else args->port = port;
	}
	return 0;
err:
	printf("Failed in arg parse!\n");
	return 1;
}

char recvBuf[4096];

Game game;

int UI(){

	char tmp = turnUI(&game);
	if(tmp == '1'){
		game.player[0]->state=p_active;
		printf("Input towards(Use arrow keys or WASD) and finished by Enter\n");
		printf("At most 3 steps!\n");
		char str[128]="MOV ";
		int p=4;
		for(;;){
			char ch = towardsUI(&game);
			if(ch=='Q') return 0;
			if(ch == 13){
				if(p != 4) break;
			}
			else if(ch == 8){
				if(p != 4) {
					str[--p] = '\0';
					
				}
			} 
			else if(p < 7){
				str[p] = ch;
				p++;
			}else printf("%c%c  %c%c",8,8,8,8);
		}
		printf("\n");
		for(int i=4; str[i]!='\0';i++) movq.push(str[i]);
		str[strlen(str)+1] = '\0';
		str[strlen(str)] = '\n';
		bufInsert(&game.send[0], str);
	}
	if(tmp == '2'){
		printf("Input towards(Use arrow keys or WASD) and finished by Enter\n");
		char str[128]="SHOOT ";
		bool flag = false;
		for(;;){
			char ch = towardsUI(&game);
			if(ch=='Q') return 0;
			if(ch == 13){
				if(flag) break;
			}
			else if(ch == 8){
				flag = false;
			} 
			else if(!flag){
				str[6] = ch;
				flag = true;
			}else printf("%c%c  %c%c",8,8,8,8);
		}		
		printf("\n");
		str[strlen(str)+1] = '\0';
		str[strlen(str)] = '\n';
		game.player[0]->state = p_onlyMove;
		bufInsert(&game.send[0], str);					
	}
	if(tmp == '3'){
		game.player[0]->equipment = equipNull;
		bufInsert(&game.send[0], "SET 0\n");
	}
	return 1;



}

int main(int argc,char* argv[]){
	string input;
	Args args,arg_default;
	pthread_t readthread;
	arg_parse(&args,argc,argv);
	
	arg_default.ip = INADDR_ANY;
	arg_default.port = htons(0);
#ifdef WIN32
	WSADATA wsaData;			
	int nRc = WSAStartup(0x0202,&wsaData);	
	if(nRc){
		printf("Winsock  startup failed with error!\n");
	}
	if(wsaData.wVersion != 0x0202){
		printf("Winsock version is not correct!\n");
	}
	printf("Winsock  startup Ok!\n");
#endif

	SOCKET clientSocket;
	sockaddr_in serverAddr,clientAddr;

	int addrLen;

	//create socket
	clientSocket = socket(AF_INET,SOCK_STREAM,0);

	if(clientSocket != INVALID_SOCKET)
		printf("Socket create Ok!\n");

	//set client port and ip
	clientAddr.sin_family = AF_INET;
	
	Set_addr(&clientAddr,arg_default);
	 
	//binding
	int rtn = bind(clientSocket,(LPSOCKADDR)&clientAddr,sizeof(clientAddr));
	if(rtn != SOCKET_ERROR)
		printf("Socket bind Ok!\n");

	serverAddr.sin_family = AF_INET;
	Set_addr(&serverAddr,args);

	rtn = connect(clientSocket,(LPSOCKADDR)&serverAddr,sizeof(serverAddr));
	if(rtn == SOCKET_ERROR )
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
			CLS();
			mapLog(&game);
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
			CLS();
			mapLog(&game);
			
			while(!UI()) printf("\n");
			
			break;
		case msg_ERR:
			printf("Disconnected from server for some reason.\n");
			if(mInt == 1) printf("Your opposite is leaving!\n");
			else printf("Unknown reason.");
			bufInsert(&game.send[0], "ERR 0\n");
			break;
		}
		

		abandonMessage(&game.recv[0]);
	}while(!game.finished);
	closesocket(clientSocket);
#ifdef WIN32
	WSACleanup();
#endif
	printf("Input Q to leave!\n");
	while(getchar()!='q'&&getchar()!='Q');
}
//10.12.56.97

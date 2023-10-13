#include "winsock2.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <pthread.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

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
	
	(addr->sin_addr).S_un.S_addr = args.ip;
	return 0;
}

char recvBuf[4096];

pthread_mutex_t lock1;

void* recvSocket(void* socket)
{
    char recvBuf[4096];
    while(1){
        FD_SET rfds,wfds;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET((SOCKET) socket,&rfds);
		FD_SET((SOCKET) socket,&wfds);
		int nTotal = select(0, &rfds, &wfds, NULL, NULL);
		if(nTotal > 0){
			pthread_mutex_lock(&lock1);
			if(FD_ISSET(socket, &rfds) ){
				memset(recvBuf, '\0', 4096);
				int rtn = recv((SOCKET) socket, recvBuf, 256, 0);
				if (rtn > 0) {
					printf("Received %d bytes : %s\n", rtn, recvBuf);
				}else{
					printf("fail!\n");
					return NULL;
				}
			}
			pthread_mutex_unlock(&lock1);
		}
    }
	return NULL;
}

int main(int argc,char* argv[]){
	pthread_mutex_init(&lock1,NULL);
	WSADATA wsaData;
	string input;
	Args args;
	pthread_t readthread;
	arg_parse(&args,argc,argv);

	int nRc = WSAStartup(0x0202,&wsaData);

	if(nRc){
		printf("Winsock  startup failed with error!\n");
	}

	if(wsaData.wVersion != 0x0202){
		printf("Winsock version is not correct!\n");
	}

	printf("Winsock  startup Ok!\n");


	SOCKET clientSocket;
	sockaddr_in serverAddr,clientAddr;

	int addrLen;

	//create socket
	clientSocket = socket(AF_INET,SOCK_STREAM,0);

	if(clientSocket != INVALID_SOCKET)
		printf("Socket create Ok!\n");

	//set client port and ip
	clientAddr.sin_family = AF_INET;
	
	clientAddr.sin_port = htons(0);
	 clientAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	 
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
	rtn = pthread_create(&readthread,NULL,recvSocket,(void*) clientSocket);
	if(rtn!=0){
		printf("Error in pthread_create!\n");
	}

		printf("Connect to server ok!");

	do {
		cout << "\nPlease input your message:";
		getline(cin, input);

		//send data to server
		rtn = send(clientSocket,input.c_str(),input.length(),0);
		if (rtn == SOCKET_ERROR) {
			printf("Send to server failed\n");
			closesocket(clientSocket);
			WSACleanup();
			return 0;
		}
	}while(input != "quit");
	closesocket(clientSocket);
	WSACleanup();
}
//10.12.56.97

#include "Socket.h"

int bufInsert(SocketBuf* socketBuf, char* str, int len){
    pthread_mutex_lock(&socketBuf->socketMutex);
    messageInserst(&socketBuf->messageList, messageStrDup(str));
    pthread_mutex_unlock(&socketBuf->socketMutex);
    return 0;
}

int socketInit(SocketBuf* socketBuf, SOCKET socket, LoopFunc loopFunc){
    socketBuf->socket = socket;
    messageListInit(&socketBuf->messageList);
    pthread_mutex_init(&socketBuf->socketMutex,NULL);
    pthread_create(&socketBuf->loopThread, NULL, loopFunc, (void*) socketBuf);
    return 0;
}

void* socketRecvLoop(void* ctx){
    SocketBuf* socketBuf = (SocketBuf*) ctx;
    char recvBuf[maxBufLength];
    while(1){
        FD_SET rfds,wfds;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(socketBuf->socket,&rfds);
		FD_SET(socketBuf->socket,&wfds);
		int nTotal = select(0, &rfds, &wfds, NULL, NULL);
		if(nTotal > 0){
			if(FD_ISSET(socket, &rfds) ){
                pthread_mutex_lock(&socketBuf->socketMutex);
				int rtn = recv(socketBuf->socket, recvBuf, 256, 0);
				if (rtn > 0) {
					bufInsert(socketBuf, recvBuf, rtn);
				}else{
                    char text[] = "ERR 1\n";
					bufInsert(socketBuf, text, strlen(text));
                    pthread_mutex_unlock(&socketBuf->socketMutex);
                    pthread_exit(NULL);
				}
                pthread_mutex_unlock(&socketBuf->socketMutex);
			}
			
		}
    }
	pthread_exit(NULL);
}

void* socketSendLoop(void* ctx){
    SocketBuf* socketBuf = (SocketBuf*) ctx;
    while(1){
        sem_wait(&socketBuf->messageList.length);
        pthread_mutex_lock(&socketBuf->socketMutex);
        send(socketBuf->socket, socketBuf->messageList.head->message, messageLength(socketBuf->messageList.head->message), 0);
        messagePop(&socketBuf->messageList);
        pthread_mutex_unlock(&socketBuf->socketMutex);
    }
}

char* pullMessage(SocketBuf* socketBuf){
    sem_wait(&socketBuf->messageList.length);
    return socketBuf->messageList.head->message;
}

int abandonMessage(SocketBuf* socketBuf){
    pthread_mutex_lock(&socketBuf->socketMutex);
    messagePop(&socketBuf->messageList);
    pthread_mutex_unlock(&socketBuf->socketMutex);
    return 0;
}
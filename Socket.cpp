#include "Socket.h"
#include <stdio.h>

int Set_addr(sockaddr_in *addr,Args args){
	addr->sin_port = htons(args.port);
#ifdef WIN32
	addr->sin_addr.S_un.S_addr = args.ip;
#endif
#ifdef __linux__
	addr->sin_addr.s_addr = args.ip;
#endif
	return 0;
}

int bufInsert(SocketBuf* socketBuf, const char* str){
    pthread_mutex_lock(&socketBuf->socketMutex);
    for(const char *ptr = str; *ptr != '\0'; ptr += messageLength(str) ){
        messageInsert(&socketBuf->messageList, messageStrDup(ptr));
    }
    pthread_mutex_unlock(&socketBuf->socketMutex);
    return 0;
}

int socketInit(SocketBuf* socketBuf, SOCKET socket){
    socketBuf->socket = socket;
    messageListInit(&socketBuf->messageList);
    pthread_mutexattr_t mutexattr;
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_PRIVATE);
    pthread_mutex_init(&socketBuf->socketMutex, &mutexattr);
    //pthread_create(&socketBuf->loopThread, NULL, loopFunc, (void*) socketBuf);
    return 0;
}
int socketStart(SocketBuf* socketBuf, LoopFunc loopFunc){
    pthread_create(&socketBuf->loopThread, NULL, loopFunc, (void*) socketBuf);
    return 0;
}

void* socketRecvLoop(void* ctx){
    SocketBuf* socketBuf = (SocketBuf*) ctx;
    DEBUG("Enter recv loop %d\n", socketBuf->socket);
    char recvBuf[maxBufLength];
    while(1){

        DEBUG("Receive recv %d\n", socketBuf->socket);
		int rtn = recv(socketBuf->socket, recvBuf, 128, 0);
        DEBUG("recv :%s !!\n" ,recvBuf);
		if (rtn > 0) {
            recvBuf[rtn] = '\0';
			bufInsert(socketBuf, recvBuf);
		}else{
			bufInsert(socketBuf, "ERR 1\n");
            pthread_mutex_unlock(&socketBuf->socketMutex);
            pthread_exit(NULL);
		}
			
    }
	pthread_exit(NULL);
}

void* socketSendLoop(void* ctx){
    SocketBuf* socketBuf = (SocketBuf*) ctx;
    DEBUG("Enter send loop %d\n", socketBuf->socket);
    while(1){
        //DEBUG("sem_wait: %x\n", &socketBuf->messageList.length);
        
        sem_wait(&socketBuf->messageList.length);
        pthread_mutex_lock(&socketBuf->socketMutex);
        DEBUG("send something: %d %s\n", socketBuf->socket, socketBuf->messageList.head->message);
        
        send(socketBuf->socket, socketBuf->messageList.head->message, 128, MSG_NOSIGNAL);
        messagePop(&socketBuf->messageList);
        pthread_mutex_unlock(&socketBuf->socketMutex);
    }
}

char* pullMessage(SocketBuf* socketBuf){
    DEBUG("sem_wait: %x\n", &socketBuf->messageList.length);
    
    sem_wait(&socketBuf->messageList.length);

    pthread_mutex_lock(&socketBuf->socketMutex);
    char* rtn = socketBuf->messageList.head->message;
    pthread_mutex_unlock(&socketBuf->socketMutex);

    return rtn;
}

int abandonMessage(SocketBuf* socketBuf){
    pthread_mutex_lock(&socketBuf->socketMutex);
    messagePop(&socketBuf->messageList);
    pthread_mutex_unlock(&socketBuf->socketMutex);
    return 0;
}
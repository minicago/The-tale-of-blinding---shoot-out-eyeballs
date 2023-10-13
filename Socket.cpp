#include "Socket.h"

int bufInsert(SocketBuf* socketBuf, const char* str, int len){
    pthread_mutex_lock(&socketBuf->socketMutex);
    for(int i = 0; i < len; i++){
        socketBuf->buf[socketBuf->length++] = str[i];
        if(socketBuf->length >= maxBufLength) {
            pthread_mutex_unlock(& socketBuf->socketMutex);
            return -1;
        }
    }
    pthread_mutex_unlock(&socketBuf->socketMutex);
    return 0;
}

int socketInit(SocketBuf* socketBuf, SOCKET socket){
    socketBuf->length = 0;
    socketBuf->socket = socket;
    pthread_mutex_init(&socketBuf->socketMutex,NULL);
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
			pthread_mutex_lock(&socketBuf->socketMutex);
			if(FD_ISSET(socket, &rfds) ){
				int rtn = recv(socketBuf->socket, recvBuf, 256, 0);
				if (rtn > 0) {
					bufInsert(socketBuf, recvBuf, rtn);
				}else{
                    char text[] = "ERR 1\n";
					bufInsert(socketBuf, text, strlen(text));
                    pthread_mutex_unlock(&socketBuf->socketMutex);
					return NULL;
				}
			}
			pthread_mutex_unlock(&socketBuf->socketMutex);
		}
    }
	return NULL;
}

int socketSend(SocketBuf* socketBuf){
    pthread_mutex_lock(&socketBuf->socketMutex);
    int rtn = send(socketBuf->socket, socketBuf->buf, socketBuf->length, 0);
    if(rtn == socketBuf->length){
        rtn = 0;
        socketBuf->length = 0;
    }else rtn = -1;
    pthread_mutex_unlock(&socketBuf->socketMutex);
    return rtn;
}
#ifndef Socket__h
#define Socket__h

#include "winsock2.h"
#include "Message.h"
#include <pthread.h>
#pragma comment(lib,"ws2_32.lib")

const int maxBufLength = 4096;

struct SocketBuf{
    SOCKET socket;
    char buf[maxBufLength];
    int length;
    pthread_mutex_t socketMutex;
};

int bufInsert(SocketBuf* socketBuf, const char* str, int len);

int socketInit(SocketBuf* socketBuf, SOCKET socket);

void* socketRecvLoop(void* ctx);

int socketSend(SocketBuf* socketBuf);

#endif
#ifndef Socket__h
#define Socket__h

#include "winsock2.h"
#include "Message.h"
#include <pthread.h>
#pragma comment(lib,"ws2_32.lib")

const int maxBufLength = 4096;
const int maxRecycleLength = 1024;

const int maxWaitSec = 120;

struct SocketBuf{
    SOCKET socket;
    MessageList messageList;
    pthread_mutex_t socketMutex;
    pthread_t loopThread;
};

int bufInsert(SocketBuf* socketBuf, const char* str);

typedef void*(LoopFunc)(void* ctx);

int socketInit(SocketBuf* socketBuf, SOCKET socket, LoopFunc loopFunc );

void* socketRecvLoop(void* ctx);

void* socketSendLoop(void* ctx);

char* pullMessage(SocketBuf* socketBuf);

int abandonMessage(SocketBuf* socketBuf);

#endif
#ifndef Socket__h
#define Socket__h

#include "Message.h"
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#pragma comment(lib,"ws2_32.lib")

const int maxBufLength = 4096;
const int maxRecycleLength = 1024;

const int maxWaitSec = 120;

typedef int SOCKET;

struct SocketBuf{
    SOCKET socket;
    MessageList messageList;
    pthread_mutex_t socketMutex;
    pthread_t loopThread;
};

int bufInsert(SocketBuf* socketBuf, const char* str);

typedef void*(LoopFunc)(void* ctx);

int socketInit(SocketBuf* socketBuf, SOCKET socket);

int socketStart(SocketBuf* socketBuf, LoopFunc loopFunc );

void* socketRecvLoop(void* ctx);

void* socketSendLoop(void* ctx);

char* pullMessage(SocketBuf* socketBuf);

int abandonMessage(SocketBuf* socketBuf);

#endif
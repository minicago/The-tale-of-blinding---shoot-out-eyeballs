#ifndef Socket__h
#define Socket__h

#include "Message.h"
#include <pthread.h>

#ifdef WIN32

#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")

#endif

#ifdef __linux__
typedef int SOCKET;
#define closesocket(x) close(x)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef sockaddr* (LPSOCKADDR);
#endif

const int maxBufLength = 4096;
const int maxRecycleLength = 1024;

const int maxWaitSec = 120;

struct Args{
	unsigned int ip;
	unsigned short port;
};

int Set_addr(sockaddr_in *addr,Args args);

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
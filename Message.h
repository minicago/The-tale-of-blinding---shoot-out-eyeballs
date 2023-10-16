#ifndef Message__h
#define Message__h

#ifdef RELEASE
#define DEBUG(...) 
#define SHOW(X)
#else
#define DEBUG(...) printf(__VA_ARGS__)
#define SHOW(X) printf(#X"=%d",(X))
#endif
#include <semaphore.h>
#pragma comment(lib, "pthreadVC2.lib")

enum MessageType{
    msg_UNKNOWN = 0,
    msg_NULL = 1,
    msg_MOV = 2,
    msg_SHOOT = 3,
    msg_STOP = 4,
    msg_SET = 5,
    msg_VOICE = 6,
    msg_TEXT = 7,
    msg_WIN = 8,
    msg_LOSE = 9,
    msg_CANCEL = 10,
    msg_TIMEOUT = 11,
    msg_READY = 12,
    msg_POSITIONX = 13,
    msg_POSITIONY = 14,
    msg_MAPSIZE = 15,
};

struct Message{
    char* message;
    Message* nxt;
};

struct MessageList{
    Message *head,*tail;
    sem_t length;
};

int messageInsert(MessageList *messageList, char* message);

int messagePop(MessageList *messageList);

char* messageStrDup(const char * message);

int messageListInit(MessageList *messageList);

bool strPreCmp(const char* str, const char* pre);

MessageType messageParse(const char *message);

int messageInt(const char *message);

int messageLength(const char *message);

const char* messageString(const char *message);

void stringLog(const char *str);

#endif
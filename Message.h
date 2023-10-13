#ifndef Message__h
#define Message__h

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
    msg_LOSE = 9
};

bool strPreCmp(const char* str, const char* pre);

MessageType messageParse(const char *message);

int messageInt(const char *message);

int messageLength(const char *message);

const char* messageString(const char *message);

void stringLog(const char *str);

#endif
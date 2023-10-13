#include "Message.h"
#include <string.h>
#include <stdio.h>
#define MSGCMPCASE(str1, type) if(strPreCmp(str1, #type)) return msg_##type

bool strPreCmp(const char* str, const char* pre){
    for(int i = 0; i < strlen(pre); i++){
        if(str[i] != pre[i]) return false;
    }
    return true;
}

MessageType messageParse(const char *message){
    MSGCMPCASE(message,NULL);
    MSGCMPCASE(message,MOV);
    MSGCMPCASE(message,SHOOT);
    MSGCMPCASE(message,STOP);
    MSGCMPCASE(message,SET);
    MSGCMPCASE(message,VOICE);
    MSGCMPCASE(message,TEXT);
    MSGCMPCASE(message,WIN);
    MSGCMPCASE(message,LOSE);
    return msg_UNKNOWN;
}

int messageLength(const char *message){
    for(int i = 0; ; i++){
        if(message[i] == '\n') return i+1;
    }
}

int messageInt(const char *message){
    int rtn = 0;
    bool flag = 0;
    for(int i = 0; ; i++){
        if(flag){
            if(message[i] == '\n') return rtn;
            else rtn = rtn * 10 + message[i] - '0';
        }
        if(message[i] == ' ') flag = 1;
    }
    return 0;
}

const char* messageString(const char *message){
    for(int i = 0; ; i++){
        if(message[i] == ' ') return (message + i + 1);
    }    
}

void stringLog(const char *str){
    for(int i = 0; ;i++){
        if(str[i]=='\n' || str[i]==' ' || str[i]=='\0') break;
        if(str[i]=='_') putchar(' ');
        else putchar(str[i]);
    }
    putchar('\n');
}
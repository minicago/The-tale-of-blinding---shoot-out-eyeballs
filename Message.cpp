#include "Message.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
    MSGCMPCASE(message,CANCEL);
    MSGCMPCASE(message,TIMEOUT);

    return msg_UNKNOWN;
}

int messageLength(const char *message){
    for(int i = 0; ; i++){
        if(message[i] == '\n'||message[i] == '\0') return i+1;
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

int messageInserst(MessageList* messageList, char * message){
    sem_post(&messageList->length);
    if(messageList->head == NULL){
        messageList->head = new Message();
        messageList->head->message = message;
        messageList->tail = messageList->head;
        messageList->head->nxt = NULL;
    }else{
        messageList->tail->nxt = new Message();
        messageList->tail = messageList->tail->nxt;
        messageList->tail->message = message;
        messageList->tail->nxt = NULL;
    }
    return 0;
}

int messagePop(MessageList *messageList){
    if(messageList->head == NULL) return -1;
    free(messageList->head->message);
    Message *tmp= messageList->head;
    messageList->head = messageList->head->nxt;
    free(tmp);
    return 0;
}

char* messageStrDup(const char * message){
    int length = messageLength(message);
    char* str = (char*) malloc(length + 1);
    for(int i = 0; i < length; i++)
        str[i] = message[i];
    str[length] = '\0';
    return str;
}

int messageListInit(MessageList *messageList){
    sem_init(&messageList->length, 1, 0);
    messageList->head = NULL;
    return 0;
}

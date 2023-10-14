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
    MSGCMPCASE(message,READY);
    MSGCMPCASE(message,POSITIONX);
    MSGCMPCASE(message,POSITIONY);
    MSGCMPCASE(message,MAPSIZE);
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

int messageInsert(MessageList* messageList, char * message){
    //printf("Begin Head:%x\n", messageList->head);
    printf("sem_post: %x\n", &messageList->length);
    sem_post(&messageList->length);
    if(messageList->head == NULL){
        messageList->head = (Message*) malloc(sizeof(Message));
        messageList->head->message = message;
        messageList->tail = messageList->head;
        messageList->head->nxt = NULL;
    }else{
        messageList->tail->nxt = (Message*) malloc(sizeof(Message));
        messageList->tail = messageList->tail->nxt;
        messageList->tail->message = message;
        messageList->tail->nxt = NULL;
    }
    //printf("End Head:%x\n", messageList->head);
    return 0;
}

int messagePop(MessageList *messageList){
    printf("messagePop:%x %s\n", messageList->head, messageList->head->message);
    if(messageList->head == NULL) return -1;
    free(messageList->head->message);
    //printf("free 1\n");
    Message *tmp= messageList->head;
    messageList->head = messageList->head->nxt;
    free(tmp);
    //printf("free 2\n");
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
    //printf("sem_init: %x\n", &messageList->length);
    int t=sem_init(&messageList->length, 0, 0);
    messageList->head = NULL;
    return 0;
}

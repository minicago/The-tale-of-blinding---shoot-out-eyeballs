#ifndef Game__h
#define Game__h

#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")

#define opposite(X) ( (X) ^ 1)
#define delta(x, y) ( (x) < (y) ? (y) - (x) : (x) - (y))
#define dist(x_1, y_1, x_2, y_2) ( delta(x_1, x_2) + delta(y_1, y_2) ) 

const int maxSize = 12;

enum PlayerState{
    dead = 0,
    onlyMove = 1,
    active = 2
};

enum Equipment{
    equipNull = 0,
    equipBell = 1
};

struct Player{
    int positionX,positionY;
    PlayerState state;
    Equipment equipment;
};

enum BlockState{
    blocked = 0,
    empty = 1,
    bell = 2
};

const int maxBufSize = 4096;

struct Game{
    int mapSize;
    BlockState map[maxSize][maxSize];
    Player* player[2];
    int currentTurn;
    char sendbuf[2][maxBufSize];
    int buflength[2];
    SOCKET socket[2];
};

const int voiceRange[]={
    0, //null
    2, //footsteps
    3, //bump
    1, //arrow
    maxSize*2 //dingdong
};

const char voiceText[][maxBufSize]={
    {"BUG unknown_voice\n"},
    {"VOICE footsteps\n"},
    {"VOICE bump\n"},
    {"VOICE arrow\n"},
    {"VOICE dingdong\n"}
};

enum Voice{
    voiceNull = 0,
    footsteps = 1,
    bump = 2,
    arrow = 3,
    dingdong = 4,
};

BlockState accessible(Game* game, int positionX, int positionY);

bool playerMove(Game *game, int playerNum, int deltaX, int deltaY);

bool captureVoice
(Game *game, int playerNum,
int postionX, int postionY,
Voice type, int range);

bool allCaptureVoice
(Game *game,
int postionX, int postionY,
Voice type, int range);

bool playerShoot(Game *game, int playerNum, int deltaX,int deltaY);

bool playerSetBell(Game *game, int playerNum);

bool locate(Game *game, int playerNum, int positionX, int positionY);

int bufAdd(Game *game, const char* str, int len, int playerNum);

int initGame(Game *game, SOCKET socket0, SOCKET socket1);

void* gameLoop(void* ctx);

#endif

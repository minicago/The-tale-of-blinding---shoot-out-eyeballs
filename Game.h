#ifndef Game__h
#define Game__h

#include "winsock2.h"
#include "Socket.h"
#pragma comment(lib,"ws2_32.lib")

#define opposite(X) ( (X) ^ 1)
#define delta(x, y) ( (x) < (y) ? (y) - (x) : (x) - (y))
#define dist(x_1, y_1, x_2, y_2) ( delta(x_1, x_2) + delta(y_1, y_2) ) 

const int maxSize = 20;

enum PlayerState{
    p_dead = 0,
    p_onlyMove = 1,
    p_active = 2
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
    m_blocked = 0,
    m_unknown = 1,
    m_empty = 2,
    m_bell = 3,
};

const int maxBufSize = 4096;



struct Game{
    int mapSize;
    BlockState map[maxSize][maxSize];
    Player* player[2];
    int currentTurn;
    SocketBuf send[2],recv[2];
    pthread_t loopThread;
    bool finished;
    //sem_t preparedThread;
};

const int voiceRange[]={
    0, //v_null
    2, //v_footsteps
    3, //v_bump
    1, //v_arrow
    maxSize*2 //v_dingdong
};

const char voiceText[][maxBufSize]={
    {"BUG unknown_voice\n"},
    {"VOICE footsteps\n"},
    {"VOICE bump\n"},
    {"VOICE arrow\n"},
    {"VOICE dingdong\n"}
};

enum Voice{
    v_null = 0,
    v_footsteps = 1,
    v_bump = 2,
    v_arrow = 3,
    v_dingdong = 4,
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

int initGame(Game *game, int mapSize, SOCKET socket0, SOCKET socket1);

void* gameLoop(void* ctx);

int cancelGame(Game *game);

void mapLog(Game *game);

#endif

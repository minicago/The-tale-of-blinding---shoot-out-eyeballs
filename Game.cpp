#include "Game.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

BlockState accessible(Game* game, int positionX, int positionY){
    if(positionX < 0 || positionX >= game->mapSize || positionY < 0 || positionY >= game->mapSize)
        return m_blocked;
    else return game->map[positionX][positionY];
} 

bool captureVoice
(Game *game, int playerNum,
int postionX, int postionY,
Voice type, int range){
    if(dist(game->player[playerNum]->positionX, game->player[playerNum]->positionY, postionX, postionY) > range) return false;
    switch (type){
    case v_null:
        return false;
        break;
        
    default:
        bufInsert(&game->send[playerNum], voiceText[(int)type]);
        return true;
        break;
    }
}

bool allCaptureVoice
(Game *game,
int postionX, int postionY,
Voice type, int range){
    captureVoice
    (game, 0,
    postionX, postionY,
    type, range);

    captureVoice
    (game, 1,
    postionX, postionY,
    type, range);    

    return true;
}

bool playerMove(Game *game, int playerNum, int deltaX, int deltaY){
    if(game->player[playerNum]->state == p_dead) return false;
    game->player[playerNum]->state = p_active;
    if(accessible(game, game->player[playerNum]->positionX + deltaX, game->player[playerNum]->positionY + deltaY) == m_blocked){
        allCaptureVoice(game, game->player[playerNum]->positionX + deltaX, game->player[playerNum]->positionY + deltaY ,v_bump, voiceRange[v_bump]);
        bufInsert(&game->send[playerNum], "MOV 0\n");
        return false;       
    } else {
        game->player[playerNum]->positionX += deltaX;
        game->player[playerNum]->positionY += deltaY;
        allCaptureVoice(game, game->player[playerNum]->positionX, game->player[playerNum]->positionY, v_footsteps, voiceRange[v_footsteps]);
        if(accessible(game, game->player[playerNum]->positionX, game->player[playerNum]->positionY) == m_bell){
            allCaptureVoice(game, game->player[playerNum]->positionX, game->player[playerNum]->positionY, v_dingdong, voiceRange[v_dingdong]);
        }
        bufInsert(&game->send[playerNum], "MOV 1\n");
        return true;    
    }
}

bool playerShoot(Game *game, int playerNum, int deltaX, int deltaY){
    if(game->player[playerNum]->state != p_active) return false; 
    game->player[playerNum]->state = p_onlyMove;
    bool heard = 0;
    for(int nx = game->player[playerNum]->positionX , ny = game->player[playerNum]->positionY ;
    true; nx += deltaX, ny += deltaY){
        if(accessible(game,nx,ny) == m_blocked){
            allCaptureVoice(game, nx, ny, v_bump, voiceRange[v_bump]);
            break;
        }else{
            if(!heard) heard = captureVoice(game, opposite(playerNum), nx, ny, v_arrow, voiceRange[v_arrow]);
            if(locate(game, opposite(playerNum), nx, ny)) {
                game->player[playerNum]->state = p_dead;
                game->finished = true;
                bufInsert(&game->send[opposite(playerNum)], "LOSE 1\n");
                bufInsert(&game->send[playerNum], "WIN 1\n");
            }
            if(accessible(game,nx,ny) == m_bell) allCaptureVoice(game, nx, ny, v_dingdong, voiceRange[v_dingdong]);
        }
    }
    return true;
}

bool playerSetBell(Game *game, int playerNum){
    if(game->player[playerNum]->equipment != equipBell) return false;
    game->map[game->player[playerNum]->positionX][game->player[playerNum]->positionY] = m_bell;
    game->player[playerNum]->equipment = equipNull;
    return true;
}

bool locate(Game *game, int playerNum, int positionX, int positionY){
    if(game->player[playerNum]->positionX == positionX && game->player[playerNum]->positionY == positionY) return true;
    else return false;
}

int drill(Game* game, int nx, int ny, int dx, int dy){
    if(nx < 0 || nx >= game->mapSize || ny < 0 || ny >= game->mapSize) return 0;
    game->map[nx][ny] = m_empty;
    if(nx == dx && ny == dy) return 1;
    bool finished = 0;
    while(!finished){
        int rnd=rand() % 100;
        if(rnd < 10){
            finished = drill(game, nx - 1, ny, dx, dy);
        }else if(rnd < 20){
            finished = drill(game, nx, ny - 1, dx, dy);
        }else if(rnd < 60){
            finished = drill(game, nx + 1, ny, dx, dy);
        }else 
            finished = drill(game, nx, ny + 1, dx, dy);
    }
    return finished;
}

int randomizeMap(Game* game){
    game->player[0]->positionX = 1;
    game->player[0]->positionY = 1;
    game->player[1]->positionX = game -> mapSize - 2;
    game->player[1]->positionY = game -> mapSize - 2; 
    for(int i = 0; i < game->mapSize; i++)
        for(int j = 0; j < game->mapSize; j++)
        {
            if(rand() % 100 <= 20) game->map[i][j] = m_empty;
            else game->map[i][j] = m_blocked;
        }
    game->map[1][1] = m_empty;
    game->map[game -> mapSize - 2][game -> mapSize - 2] = m_empty;
    
    for(int i = 0; i < 4 ; i++){
        drill(game, 0, 0, game -> mapSize - 1, game -> mapSize - 1);
    }

    mapLog(game);

    return 0;
}

int sendPosition(Game *game, int playernum){
    char message[64];
    sprintf(message, "POSITIONX %d\n", game->player[playernum]->positionX);
    bufInsert(&game->send[playernum], message);
    sprintf(message, "POSITIONY %d\n", game->player[playernum]->positionY);
    bufInsert(&game->send[playernum], message);
    sprintf(message, "MAPSIZE %d\n", game->mapSize);
    bufInsert(&game->send[playernum], message);
    return 0;
}

int initGame(Game *game, int mapSize, SOCKET socket0, SOCKET socket1){
    
    game->currentTurn = 0;
    game->mapSize = mapSize;
    game->player[0] = (Player*) malloc(sizeof(Player));
    game->player[0]->state = p_active;
    game->player[0]->equipment = equipBell;
    game->player[1] = (Player*) malloc(sizeof(Player));
    game->player[1]->state = p_active;
    game->player[1]->equipment = equipBell;
    game->finished = false;
    randomizeMap(game);
    socketInit(&game->send[0], socket0);
    socketInit(&game->send[1], socket1);
    socketInit(&game->recv[0], socket0);
    socketInit(&game->recv[1], socket1);

    socketStart(&game->send[0], socketSendLoop);
    socketStart(&game->send[1], socketSendLoop);
    socketStart(&game->recv[0], socketRecvLoop);
    socketStart(&game->recv[1], socketRecvLoop);

    pthread_create(&game->loopThread, NULL, gameLoop, (void*) game);
    return 0;
}



void* gameLoop(void* ctx){
    Game* game = (Game*) ctx;

    //pullMessage(&game->recv[0]);
    sendPosition(game, 0);

    //abandonMessage(&game->recv[0]);

    //pullMessage(&game->recv[1]);
    sendPosition(game, 1); 
    //abandonMessage(&game->recv[1]);
    
      
    while(!game->finished){
        mapLog(game);
        game->currentTurn ++;
        printf("Turn: %d\n",  game->currentTurn );
        int currentPlayer = game->currentTurn & 1;
        bufInsert(&game->send[currentPlayer], "READY 0\n");
        char *message;
        while(1){

            message = pullMessage(&game->recv[currentPlayer]);

            DEBUG("pull message: %s\n",message);
            if(messageParse(message) == msg_TIMEOUT){
                abandonMessage(&game->recv[currentPlayer]);
                if(game->recv[currentPlayer].messageList.head == NULL){
                    bufInsert(&game->send[0],"CANCEL 1\n");
                    bufInsert(&game->send[1],"CANCEL 1\n");
                    cancelGame(game);
                }
            }else break;
        }
        MessageType msgType = messageParse(message);
        const char* mArg = messageString(message);
        switch (msgType)
        {
        case msg_MOV:
            
            for(int cnt = 0; cnt < 3; cnt++){
                if(mArg[cnt] == '\n' || mArg[cnt] == '\0') break;           
                if(mArg[cnt] == 'U'){
                    if(playerMove(game, currentPlayer, -1, 0) == false) break;
                }
                if(mArg[cnt] == 'D'){
                    if(playerMove(game, currentPlayer, +1, 0) == false) break;
                }                
                if(mArg[cnt] == 'L'){
                    if(playerMove(game, currentPlayer, 0, -1) == false) break;
                }
                if(mArg[cnt] == 'R'){
                    if(playerMove(game, currentPlayer, 0, +1) == false) break;
                }                                
            }
            break;
        
        case msg_SHOOT:
            if(mArg[0] == 'U'){
               playerShoot(game, currentPlayer, -1, 0);
            }
            if(mArg[0] == 'D'){
                playerShoot(game, currentPlayer, +1, 0);
            }                
            if(mArg[0] == 'L'){
                playerShoot(game, currentPlayer, 0, -1);
            }
            if(mArg[0] == 'R'){
                playerShoot(game, currentPlayer, 0, +1);
            }             
            break;

        case msg_SET:
            playerSetBell(game, currentPlayer);            
            break;

        default:
            break;
        }
        abandonMessage(&game->recv[currentPlayer]);
        bufInsert(&game->send[currentPlayer], "TEXT 0\n");
        
    }
    
    pullMessage(&game->recv[0]);
    pullMessage(&game->recv[1]);
    abandonMessage(&game->recv[0]);
    abandonMessage(&game->recv[1]);
    cancelGame(game);
    pthread_exit(NULL);
    return NULL;
}

int cancelGame(Game *game){
    pthread_cancel(game->loopThread);
    pthread_cancel(game->recv[0].loopThread);
    pthread_cancel(game->recv[1].loopThread);
    pthread_cancel(game->send[0].loopThread);
    pthread_cancel(game->send[1].loopThread);    
    //send socket & recv socket shares one
    closesocket(game->send[0].socket); 
    closesocket(game->send[1].socket);
    free(game->player[0]);
    free(game->player[1]);
    game->finished = true;
    return 0;
}

void mapLog(Game *game){
    for(int i = 0; i < game->mapSize; i++)
    {
        for(int j = 0; j < game->mapSize; j++)
        {
            if(locate(game, 0, i, j)) putchar('O');
            else if(locate(game, 1, i, j)) putchar('*');
            else if(game->map[i][j] == m_empty) putchar('.');
            else if(game->map[i][j] == m_unknown) putchar('?');
            else putchar('#');
        }
        putchar('\n');
    } 
}

int initClient(Game *game, SOCKET socket){
    DEBUG("initialized start\n");
    game->currentTurn = 0;
    game->player[0] = (Player*) malloc(sizeof(Player));
    game->player[0]->state = p_active;
    game->player[0]->equipment = equipBell;
    game->player[1] = (Player*) malloc(sizeof(Player));
    game->player[1]->positionX = -1;
    game->player[1]->positionY = -1;    
    game->finished = false;
    DEBUG("game state initialized\n");
    socketInit(&game->send[0], socket);
    socketInit(&game->recv[0], socket);   
    socketStart(&game->send[0], socketSendLoop);
    socketStart(&game->recv[0], socketRecvLoop);
    DEBUG("socket initialized\n");
    return 0;
}

int nullMap(Game *game, int mapSize){
    game->mapSize = mapSize;
    for(int i = 0; i < mapSize; i++)
        for(int j = 0; j < mapSize; j++)
            game->map[i][j] = m_unknown;
    game->map[1][1] = m_empty;
    game->map[mapSize-2][mapSize-2] = m_empty;
    return 0;
}
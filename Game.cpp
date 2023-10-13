#include "Game.h"
#include <stdio.h>
#include <string.h>

BlockState accessible(Game* game,int positionX,int positionY){
    if(positionX < 0 || positionX >= game->mapSize || positionY < 0 || positionY >= game->mapSize)
        return blocked;
    else return game->map[positionX][positionY];
} 

bool captureVoice
(Game *game, int playerNum,
int postionX, int postionY,
Voice type, int range){
    if(dist(game->player[playerNum]->positionX, game->player[playerNum]->positionY, postionX, postionY) > range) return false;
    switch (type){
    case voiceNull:
        return false;
        break;
        
    default:
        bufAdd(game, voiceText[(int)type], strlen(voiceText[(int)type]), playerNum);
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
    if(game->player[playerNum]->state == dead) return false;
    game->player[playerNum]->state = active;
    if(accessible(game, game->player[playerNum]->positionX, game->player[playerNum]->positionY) == blocked){
        captureVoice(game, opposite(playerNum), game->player[playerNum]->positionX + deltaX, game->player[playerNum]->positionY + deltaY ,bump, voiceRange[bump]);
        char text[]="MOVE 0\n";
        bufAdd(game, text, strlen(text), playerNum);
        return false;       
    } else {
        game->player[playerNum]->positionX += deltaX;
        game->player[playerNum]->positionY += deltaY;
        allCaptureVoice(game, game->player[playerNum]->positionX, game->player[playerNum]->positionY, footsteps, voiceRange[footsteps]);
        if(accessible(game, game->player[playerNum]->positionX, game->player[playerNum]->positionY) == bell){
            allCaptureVoice(game, game->player[playerNum]->positionX, game->player[playerNum]->positionY, dingdong, voiceRange[dingdong]);
        }
        char text[]="MOVE 1\n";
        bufAdd(game, text, strlen(text), playerNum);        
        return true;    
    }
}

bool playerShoot(Game *game, int playerNum, int deltaX, int deltaY){
    if(game->player[playerNum]->state != active) return false; 
    game->player[playerNum]->state = onlyMove;
    bool heard = 0;
    for(int nx = game->player[playerNum]->positionX + deltaX , ny = game->player[playerNum]->positionY + deltaY;
    true; nx += deltaX, ny += deltaY){
        if(accessible(game,nx,ny) == blocked){
            allCaptureVoice(game, nx, ny, bump, voiceRange[bump]);
            break;
        }else{
            if(!heard) heard = captureVoice(game, opposite(playerNum), nx, ny, arrow, voiceRange[arrow]);
            if(locate(game, opposite(playerNum), nx, ny)) {
                game->player[playerNum]->state = dead;
                char text1[] = "DEAD 1\n";
                bufAdd(game, text1, strlen(text1), opposite(playerNum));
                char text2[]= "WIN 1\n";
                bufAdd(game, text2, strlen(text2), playerNum);
            }
            if(accessible(game,nx,ny) == bell) allCaptureVoice(game, nx, ny, dingdong, voiceRange[dingdong]);
        }
    }
    return true;
}

bool playerSetBell(Game *game, int playerNum){
    if(game->player[playerNum]->equipment != equipBell) return false;
    game->map[game->player[playerNum]->positionX][game->player[playerNum]->positionY] = bell;
    game->player[playerNum]->equipment = equipNull;
    return true;
}

bool locate(Game *game, int playerNum, int positionX, int positionY){
    if(game->player[playerNum]->positionX == positionX && game->player[playerNum]->positionY == positionY) return true;
    else return false;
}

int bufAdd(Game *game, const char* str, int len, int playerNum){
    for(int i = 0; i < len; i++){
        game->sendbuf[playerNum][game->buflength[playerNum]++] = str[i];
        if(game->buflength[playerNum] >= maxBufSize) return -1;
    }
    return 0;
}


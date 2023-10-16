#include "UI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <conio.h>
#endif
char turnUI(Game *game){
    

    bool KEYS[256];
    memset(KEYS, 0, sizeof(KEYS));
    printf("********************\n");
    printf("* 1 MOVE           *\n");
    KEYS['1'] = 1;
    if(game->player[0]->state == p_active){
    printf("* 2 SHOOT          *\n");
    KEYS['2'] = 1;
    }
    if(game->player[0]->equipment == equipBell){
    printf("* 3 SET BELL       *\n");
    KEYS['3'] = 1;
    }
    printf("********************\n");

    while(true){
#ifdef WIN32
        char ch = getch();
#endif
#ifdef __linux__
        char ch = getchar();
#endif
        if(KEYS[ch]) return ch; 
    }



}

char towardsUI(Game *game){

    

    bool arrow = 0;
    while(true){
        #ifdef WIN32
            char ch = getch();
        #endif
        #ifdef __linux__
            char ch = getchar();
        #endif
        
        if(!arrow && (ch == 'w' || ch == 'W') || ch == 72 && arrow){
            printf("%c%c",161,252);
            return 'U';
        }
        if(!arrow && (ch == 's' || ch == 'S') || ch == 80 && arrow){
            printf("%c%c",161,253);
            return 'D';
        }
        if(!arrow && (ch == 'a' || ch == 'A') || ch == 75 && arrow){
            printf("%c%c",161,251);
            return 'L';
        }
        if(!arrow && (ch == 'd' || ch == 'D') || ch == 77 && arrow){
            printf("%c%c",161,250);
            return 'R';
        }
        if(!arrow && (ch=='q' || ch == 'Q') ) return 'Q';
        if(ch == 8){
            printf("%c%c  %c%c",8,8,8,8);
            return 8;
        }
        if(ch == 13){
            return 13;
        }

        if(ch == 0xE0) arrow = 1;
        else arrow = 0;
    }

}

void CLS(){
    printf("******************************\n");
    //do nothing
}

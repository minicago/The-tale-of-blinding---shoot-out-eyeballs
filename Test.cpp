#include "Game.h"
#include <stdio.h>
Game g;

int main(){
    initGame(&g, 12, 0 ,0);

    mapLog(&g);
}
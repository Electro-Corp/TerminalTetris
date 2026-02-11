#include "block.h"

// Returns a (kinda) random block
G_Block blockCreateNewBlock(){
    int num = (rand() % (7 + 1));
    G_Block block = {shapes[num], {5, 7}};
    return block;
}

// Rotates a block in a direction (1 = CW, -1 = CCW)
G_Block blockRotateBlock(G_Block block, int dir){
    G_Block rotated = block; // Create copy

    for(int i = 0; i < 4; i++){
        G_Position originalPos = rotated.shape.spaces[i];
        int x = originalPos.x, y = originalPos.y;
        originalPos.y = x * -1 * dir;
        originalPos.x = y;

        rotated.shape.spaces[i] = originalPos;
    }

    return rotated;
}
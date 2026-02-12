#include "block.h"

// Returns a (kinda) random block
G_Block blockCreateNewBlock(){
    int num = (rand() % (6 + 1));
    G_Block block = {shapes[num], {5, 2}};
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

// Get furthest point on the block (1 = Rightward, 0 = Leftward, 2 = Downward)
G_Position blockGetExtremeOnBlock(G_Block block, int dir){
    G_Position extreme = {0, 0};
    switch(dir){
        // Leftward most chunk
        case 0:
            for(int i = 0; i < 4; i++) if(extreme.x > block.shape.spaces[i].x) extreme = block.shape.spaces[i]; 
            break;
        // Righward most chunk
        case 1:
            for(int i = 0; i < 4; i++) if(extreme.x < block.shape.spaces[i].x) extreme = block.shape.spaces[i]; 
            break;
        // Downward most chunk
        case 2:
            for(int i = 0; i < 4; i++) if(extreme.y < block.shape.spaces[i].y) extreme = block.shape.spaces[i]; 
            break;
    }
    return extreme;
}
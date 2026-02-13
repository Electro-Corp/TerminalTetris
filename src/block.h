/*
    block.h 
    For each "tetromino"
*/
#ifndef BLOCK_H
#define BLOCK_H

#include <stdlib.h>

typedef struct {
    int x, y;
} G_Position;

typedef struct {
    int r, g, b;
} G_Color;

typedef struct {
    G_Position spaces[4];
    G_Color color;
} G_Shape;

typedef struct {
    G_Shape shape;
    G_Position pos;
} G_Block;

//
// Base Tetrominos
//    The center of the block is 0,0
//    These are defined with their original orientation
//

static const G_Shape OrangeRicky = {
    {
        {-1, 0},
        {0, 0},
        {1, 0},
        {1, -1},
    },
    {255, 130, 0}
};

static const G_Shape BlueRicky = {
    {
        {-1, 0},
        {0, 0},
        {1, 0},
        {-1, -1},
    },
    {30, 30, 255}
};

static const G_Shape ClevelandZ = {
    {
        {-1, 1},
        {0, 1},
        {0, 0},
        {1, 0},
    },
    {255, 30, 30}
};

static const G_Shape RhodeIslandZ = {
    {
        {-1, 0},
        {0, 1},
        {0, 0},
        {1, 1},
    },
    {30, 255, 30}
};

static const G_Shape Hero = {
    {
        {-1, 0},
        {0, 0},
        {1, 0},
        {2, 0},
    },
    {30, 60, 150}
};

static const G_Shape TeeWee = {
    {
        {-1, 0},
        {0, -1},
        {0, 0},
        {1, 0},
    },
    {100, 30, 150}
};

static const G_Shape SmashBoy = {
    {
        {-1, 0},
        {-1, -1},
        {0, 0},
        {0, -1},
    },
    {30, 100, 150}
};

static int chosen[7] = {0, 0, 0, 0, 0, 0, 0};
static G_Shape shapes[7] = {OrangeRicky, BlueRicky, ClevelandZ, RhodeIslandZ, Hero, TeeWee, SmashBoy};
static int chosenSoFar = 0;

static G_Block nextBlock, comingUp;

// Create first block
void blockSetup();

// Returns a (kinda) random block
G_Block blockCreateNewBlock();

// Get next block
G_Block blockGetNextBlock();

// Get coming up block
G_Block blockGetComingUp();

// Rotates a block in a direction (1 = CW, -1 = CCW)
G_Block blockRotateBlock(G_Block block, int dir);

// Get furthest point on the block (1 = Rightward, 0 = Leftward, 2 = Downward)
G_Position blockGetExtremeOnBlock(G_Block block, int dir);

#endif
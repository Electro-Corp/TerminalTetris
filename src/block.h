/*
    block.h 
    For each "tetromino"
*/
#ifndef BLOCK_H
#define BLOCK_H

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
        {1, 1},
    },
    {100, 100, 50}
};

static const G_Shape BlueRicky = {
    {
        {-1, 0},
        {0, 0},
        {1, 0},
        {-1, 1},
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
        {0, 1},
        {0, 0},
        {1, 0},
    },
    {100, 30, 150}
};

static const G_Shape SmashBoy = {
    {
        {-1, 0},
        {-1, 1},
        {0, 0},
        {0, 1},
    },
    {30, 100, 150}
};


static G_Shape shapes[7] = {OrangeRicky, BlueRicky, ClevelandZ, RhodeIslandZ, Hero, TeeWee, SmashBoy};

// Returns a (kinda) random block
G_Block blockCreateNewBlock();

// Rotates a block in a direction (1 = CW, -1 = CCW)
G_Block blockRotateBlock(G_Block block, int dir);

#endif
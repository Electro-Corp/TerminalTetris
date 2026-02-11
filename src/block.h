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
    G_Position spaces[5];
    G_Color color;
} G_Block;

#endif
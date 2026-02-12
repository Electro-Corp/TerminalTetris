/*
    graphics.h
    Header for all things graphics and input
*/
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "block.h"

#define TETRIS_WIDTH 10
#define TETRIS_HEIGHT 20

#define TETRIS_BACKGROUND "   "

// Two types of display modes, Menu and Game

// Menu option
typedef struct {
    char* text;
    void (*callback)();
} G_Option;

// Menu definition
typedef struct {
    int optionsCount;
    G_Option* options;
} G_Menu;

// Game

typedef struct {
    G_Color color;
    int empty;
} G_Tile;

// Map
static G_Tile map[TETRIS_HEIGHT][TETRIS_WIDTH];

// Background color
static G_Color backgroundColor = {100, 100, 100};

// Open terminal size
static int terminalWidth, terminalHeight;
// Where the game should be
static int gameXOffset, gameYOffset;

// Init graphics zone, based on current terminal size
void graphicsInit();

//
// MENUS
//

// Init a menu
void graphicsMenuLoop(G_Menu* menu);

// Free a menu
void graphicsFreeMenu(G_Menu* menu);

//
// GAME
//

// Init backdrop
void graphicsInitBackdrop();

// Draw frame
void graphicsDrawFrame(G_Block currentBlock);

// Add block to map
void graphicsAddBlockToMap(G_Block block);

// 

//
// Helpers
//

// What X-Pos to have centered text
int graphicsHelper_GetPositionToCenterText(int len);

// Move cursor to X, Y position
void graphicsHelper_CursorAt(int x, int y);

// Set terminal color
void graphicsHelper_SetColor(int r, int g, int b);

#endif
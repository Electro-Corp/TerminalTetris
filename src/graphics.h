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

#define TETRIS_WIDTH 10
#define TETRIS_HEIGHT 20

#define TETRIS_BACKGROUND " * "

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
void graphicsDrawFrame();

// Helpers

// What X-Pos to have centered text
int graphicsHelper_GetPositionToCenterText(int len);

// Move cursor to X, Y position
void graphicsHelper_CursorAt(int x, int y);

#endif
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

// Open terminal size
static int terminalWidth, terminalHeight;

// Init graphics zone, based on current terminal size
void graphicsInit();

// Init a menu
void graphicsMenuLoop(G_Menu* menu);

// Free a menu
void graphicsFreeMenu(G_Menu* menu);

// Helpers

// What X-Pos to have centered text
int graphicsHelper_GetPositionToCenterText(int len);

// Move cursor to X, Y position
void graphicsHelper_CursorAt(int x, int y);

#endif
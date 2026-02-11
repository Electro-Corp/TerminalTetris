/*
    main.c
    Main file for TerminalTetris
*/
#include <stdio.h>

// Non-std
#include "graphics.h"

// Variables
G_Menu mainMenu;

// Main menu options

// Begin Tetris
void startGame();
// Quit Game
void quit();

// Create main menu
G_Menu createMainMenu();

int main(int args, char* argv){
    // Guess
    graphicsInit();

    // Create main menu
    mainMenu = createMainMenu();
    graphicsMenuLoop(&mainMenu);

    return 0;
}

// Create main menu
G_Menu createMainMenu(){
    G_Option startGameOption = {
        "Start Game",
        &startGame
    };
    G_Option quitGameOption = {
        "Quit",
        &quit
    };

    G_Menu menu;
    
    menu.optionsCount = 2;

    menu.options = (G_Option*)malloc(sizeof(startGameOption) + sizeof(quitGameOption));
    menu.options[0] = startGameOption;
    menu.options[1] = quitGameOption;

    return menu;
}

// Begin Tetris
void startGame(){
    // Free main menu
    graphicsFreeMenu(&mainMenu);
}

// Quit Game
void quit(){
    exit(0);
}
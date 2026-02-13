/*
    main.c
    Main file for TerminalTetris
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

// Non-std
#include "graphics.h"
#include "block.h"

// Variables
G_Menu mainMenu; // Main menu
G_Menu pauseMenu; // Pause menu
int fallTick = 500; // ms for a block to fall 
int frameTime = 100; // ms to wait after a frame
struct termios originalTerm; // Original terminal settings

//
// Main menu options
//

void startGame(); // Begin Tetris
void quit(); // Quit Game

G_Menu createMainMenu(); // Create main menu
void tetrisLoop(); // Main loop of tetris
double getCurrentTimeMs(); // I wonder....

// Signals
void segfault(int);
void interrupt(int);

// Restore terminal mode
void restoreTermMode();

int main(int args, char* argv){
    // Capture original terminal settings
    tcgetattr(0, &originalTerm);
    // Setup signals
    signal(SIGINT, interrupt);
    signal(SIGSEGV, segfault);
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
    //graphicsFreeMenu(&mainMenu);
    struct termios termInfo;
    tcgetattr(0, &termInfo);
    termInfo.c_lflag &= ~ICANON;
    termInfo.c_lflag &= ~ECHO;
    termInfo.c_cc[VMIN] = 0;
    termInfo.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &termInfo);
    // Ready randomness
    srand(time(NULL));
    // Init backdrop
    graphicsInitBackdrop();
    // Loop
    tetrisLoop();
}

// Main loop for tetris game
void tetrisLoop(){
    // Generate a block
    G_Block block = blockCreateNewBlock();

    // Loop variables
    double lastMs = 0.0;
    int updateScreen = 0;

    // Draw first frame
    graphicsDrawFrame(block);
    // Main Tetris loop
    while(1){
        updateScreen = 0;
        // Check if block should fall
        if(getCurrentTimeMs() - lastMs > fallTick){
            block.pos.y++;
            lastMs = getCurrentTimeMs();
            updateScreen = 1; // Yes, update screen
        }

        // Query input
        char c;
        read(0, &c, 1);
        if(c != EOF){
            switch(c){
                // Left
                case 'a':
                    // Check if at edge
                    if((blockGetExtremeOnBlock(block, 0).x + block.pos.x) > 0){
                        updateScreen = 1;
                        block.pos.x--;
                    }
                    break;
                // Up
                case 'w':
                    updateScreen = 1;
                    block = blockRotateBlock(block, 1);
                    // Check if we need to push the block away
                    if((blockGetExtremeOnBlock(block, 1).x + block.pos.x) + 1 > TETRIS_WIDTH) block.pos.x--;
                    if((blockGetExtremeOnBlock(block, 0).x + block.pos.x) < 0) block.pos.x++;
                    if((blockGetExtremeOnBlock(block, 2).y + block.pos.y) + 1 > TETRIS_HEIGHT * 2) block.pos.y--;
                    break;
                // Right
                case 'd':
                    // Check if at edge
                    if((blockGetExtremeOnBlock(block, 1).x + block.pos.x) + 1 < TETRIS_WIDTH){
                        updateScreen = 1;
                        block.pos.x++;
                    }
                    break;
                // Down
                case 's':
                    // Check if at bottom
                    if((blockGetExtremeOnBlock(block, 2).y + block.pos.y) + 1 < TETRIS_HEIGHT){
                        updateScreen = 1;
                        block.pos.y++;
                    }
                    break;
            }
        }

        // Check if we should stick
        if(blockGetExtremeOnBlock(block, 2).y + block.pos.y == TETRIS_HEIGHT || graphicsIsHittingOtherBlock(block)){
            graphicsAddBlockToMap(block);
            block = blockCreateNewBlock();
            updateScreen = 1;
        }

        // Do we need to update the screen?
        if(updateScreen == 1){
            graphicsDrawFrame(block);
            fflush(stdout);
        }

        //usleep(200000);
        double s = getCurrentTimeMs();
        while(getCurrentTimeMs() - s < frameTime);
        //while ((c = getchar()) != '\n' && c != EOF);
    }
}

// Quit Game
void quit(){
    exit(0);
}

double getCurrentTimeMs(){
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000.0) + (time.tv_usec / 1000.0);
}

// Signals
void segfault(int){
    restoreTermMode();
    printf("Segmentation Fault... oops...\n");
    exit(-1);
}

void interrupt(int){
    restoreTermMode();
    printf("Exiting TerminalTetris.\n");
    exit(0);
}

// Restore terminal mode
void restoreTermMode(){
    tcsetattr(0, TCSANOW, &originalTerm);
    graphicsHelper_ResetTerm();
}
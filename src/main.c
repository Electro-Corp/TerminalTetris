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
int fallTick = 100; // ms for a block to fall 
int frameTime = 10; // ms to wait after a frame
int holdAtBottom = 500;
struct termios originalTerm; // Original terminal settings
FILE* highScoreFile;
HSCORE_ENTRY highScores[10];

//
// Main menu options
//

void startGame(); // Begin Tetris
void quit(); // Quit Game

//
// Game funcs
//

G_Menu createMainMenu(); // Create main menu
void tetrisLoop(); // Main loop of tetris
void gameOver(); // Game... over..
double getCurrentTimeMs(); // I wonder....

// High-score
void loadHighScore();
void dumpHighScores();

// Signals
void segfault(int);
void interrupt(int);

// Restore terminal mode
void restoreTermMode();

int main(int args, char** argv){
    loadHighScore();
    // Capture original terminal settings
    tcgetattr(0, &originalTerm);
    // Setup signals
    signal(SIGINT, interrupt);
    signal(SIGSEGV, segfault);
    // Guess
    graphicsInit();

    // Create main menu
    //mainMenu = createMainMenu(); // Dont bother creating main menu.
    //graphicsMenuLoop(&mainMenu);    // (theres nothing besides "start" and "quit", whats the point)

    startGame();

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

    menu.options = (G_Option*)malloc(sizeof(startGameOption) + sizeof(quitGameOption)); // ugly allocation
    menu.options[0] = startGameOption;
    menu.options[1] = quitGameOption;

    return menu;
}

// Begin Tetris
void startGame(){
    // Free main menu
    //graphicsFreeMenu(&mainMenu);
    // Set input mode
    struct termios termInfo;
    tcgetattr(0, &termInfo);
    termInfo.c_lflag &= ~ICANON;
    termInfo.c_lflag &= ~ECHO;
    termInfo.c_cc[VMIN] = 0;
    termInfo.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &termInfo);
    // Ready randomness
    srand(time(NULL));
    // Setup blocks
    blockSetup();
    // Init backdrop
    graphicsInitBackdrop();
    // Loop
    tetrisLoop();
}

// Main loop for tetris game
void tetrisLoop(){
    // Get a block
    G_Block block = blockGetNextBlock();
    // Loop variables
    double lastMs = 0.0, lastFrameTime = 0.0, holdBottomTime = 0.0;
    int updateScreen = 0, paused = 0, holding = 0;

    // Draw first frame
    graphicsDrawFrame(block, holding, getCurrentTimeMs() - holdBottomTime / holdAtBottom);
    // Main Tetris loop
    while(1){
        // Update fall time
        fallTick = fallTick - (fallTick * (0.1 * level));
        holdAtBottom = fallTick * 5;
        // Check if we're at the top
        if(block.pos.y == 1 && graphicsSquareHittingBook(block.pos, blockGetExtremeOnBlock(block, 2), 2)){
            break;
        }

        // Don't update unless we need to
        updateScreen = 0;

        // Check if we should stick
        if(blockGetExtremeOnBlock(block, 2).y + block.pos.y == TETRIS_HEIGHT || graphicsIsHittingOtherBlock(block)){
            if(holding && getCurrentTimeMs() - holdBottomTime > holdAtBottom){
                graphicsAddBlockToMap(block);
                block = blockGetNextBlock();
                holding = 0;
            }else if(!holding){
                holdBottomTime = getCurrentTimeMs();
                holding = 1;
            }
            updateScreen = 1;
        }else{
            holding = 0;
        }

        // Check if block should fall
        if(getCurrentTimeMs() - lastMs > fallTick && !paused && !holding){
            block.pos.y++;
            lastMs = getCurrentTimeMs();
            updateScreen = 1; // Yes, update screen
        }

        // Query input
        char c = getchar();
        read(0, &c, 1);
        // Special paused senario
        if(c == 'p'){
            // Are we already paused?
            if(paused){
                // Unpause
                updateScreen = 1;
                paused = 0;
                // Redraw background
                graphicsInitBackdrop();
            }else{
                // Pause..
                graphicsDrawPause();
                paused = 1;
            }
        }
        if(!paused){
            switch(c){
                // Left
                case 'a':
                    // Check if at edge
                    if((blockGetExtremeOnBlock(block, 0).x + block.pos.x) > 0 && !graphicsSquareHittingBook(block.pos, blockGetExtremeOnBlock(block, 0), 0)){
                        updateScreen = 1;
                        block.pos.x--;
                    }
                    break;
                // Up
                case 'w':
                    updateScreen = 1;
                    G_Block tmp = blockRotateBlock(block, 1);
                    // Check if we need to push the block away
                    if((blockGetExtremeOnBlock(tmp, 1).x + tmp.pos.x) + 1 > TETRIS_WIDTH && !graphicsSquareHittingBook(tmp.pos, blockGetExtremeOnBlock(tmp, 1), 1)){
                        tmp.pos.x--;
                    }
                    if((blockGetExtremeOnBlock(tmp, 0).x + tmp.pos.x) < 0 && !graphicsSquareHittingBook(tmp.pos, blockGetExtremeOnBlock(tmp, 0), 0)){
                        tmp.pos.x++;
                    }
                    if((blockGetExtremeOnBlock(tmp, 2).y + tmp.pos.y) - 1 > TETRIS_HEIGHT){
                        tmp.pos.y--;
                    }

                    // Are we still hitting something?
                    if(graphicsIsHittingOtherBlock(block)){
                        // Okay give up
                        tmp = block;
                    }

                    block = tmp;
                    break;
                // Right
                case 'd':
                    // Check if at edge
                    if((blockGetExtremeOnBlock(block, 1).x + block.pos.x) + 1 < TETRIS_WIDTH && !graphicsSquareHittingBook(block.pos, blockGetExtremeOnBlock(block, 1), 1)){
                        updateScreen = 1;
                        block.pos.x++;
                    }
                    break;
                // Down
                case 's':
                    // Check if at bottom
                    if((blockGetExtremeOnBlock(block, 2).y + block.pos.y) + 1 < TETRIS_HEIGHT && !graphicsIsHittingOtherBlock(block)){
                        updateScreen = 1;
                        block.pos.y++;
                    }
                    break;
                // Push block down 
                case '\n':
                case ' ':
                    int blocksFallen = 0;
                    while(!(blockGetExtremeOnBlock(block, 2).y + block.pos.y + 1 == TETRIS_HEIGHT || graphicsIsHittingOtherBlock(block))){
                        block.pos.y++;
                        blocksFallen++;
                        updateScreen = 1;
                    }
                    graphicsAddToScore(blocksFallen);
            }
        }        

    
        // Do we need to update the screen?
        if(updateScreen == 1){
            graphicsDrawFrame(block, holding, (getCurrentTimeMs() - holdBottomTime) / holdAtBottom);
            fflush(stdout);
        }

        // Wait out until frame time 
        while(getCurrentTimeMs() - lastFrameTime < frameTime);

        lastFrameTime = getCurrentTimeMs();
    }

    gameOver();
}

void gameOver(){
    graphicsDrawGameOver(&highScores);
    // Check if score should be added
    int addToBoard = -1, pScore = graphicsGetScore();
    for(int i = 0; i < 10; i++){
        if(pScore > highScores[i].score){ 
            addToBoard = i;
        }
    }
    while(getchar() != 'q');
    if(addToBoard > 0){
        restoreTermMode();
        // Get user input
        char* name = graphicsInputString("New high score! Enter your name:", NAME_MAX_SIZE);
        strcpy(highScores[addToBoard].name, name);
        highScores[addToBoard].score = pScore;
        dumpHighScores();
    }
    quit();
}

// Quit Game
void quit(){
    dumpHighScores();
    restoreTermMode();
    printf("Exiting TerminalTetris...\n");
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
    quit();
}

// Restore terminal mode
void restoreTermMode(){
    tcsetattr(0, TCSANOW, &originalTerm);
    graphicsHelper_ResetTerm();
}

// High-score stuff!
void loadHighScore(){
    highScoreFile = fopen("hscore", "r");
    if(!highScoreFile){
        for(int i = 0; i < 10; i++){
            HSCORE_ENTRY tmp;
            strcpy(&tmp.name, "None");
            tmp.score = 1;
            highScores[i] = tmp;
        }
    }else{
        // Load from file
        fread(&highScores, sizeof(HSCORE_ENTRY), 10, highScoreFile);
        fclose(highScoreFile);
    }
}

void dumpHighScores(){
    highScoreFile = fopen("hscore", "w");
    fwrite(&highScores, sizeof(HSCORE_ENTRY), 10, highScoreFile);
    fclose(highScoreFile);
}
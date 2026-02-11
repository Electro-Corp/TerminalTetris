#include "graphics.h"

// Init graphics zone, based on current terminal size
void graphicsInit(){
    // Get width and height of terminal
    struct winsize winSize;
    int err;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &winSize) < 0){
        err = errno;
        printf("Error: ioctl failed: %s\n", strerror(err));
        exit(-1);
    }else{
        terminalHeight = winSize.ws_row;
        terminalWidth = winSize.ws_col;
    }
    // Calculate where the game should be
    gameYOffset = (terminalHeight / 2) - (TETRIS_HEIGHT);
    gameXOffset = (terminalWidth / 2) - ((strlen(TETRIS_BACKGROUND) * TETRIS_WIDTH) / 2);

    printf("Terminal Size: %d %d\nGame Pos: %d %d", terminalHeight, terminalWidth, gameXOffset, gameYOffset);

    // Hide cursor
    printf("\033[?25l");
}

// Init a menu
void graphicsMenuLoop(G_Menu* menu){
    int currentOption = 0, loop = 1;
    
    char c; 
    // Loop
    while(loop){
        // Clear the screen and move to home position
        printf("\033[2J\033[H");
        fflush(stdout);
        // Render menu
        for(int i = 0; i < menu->optionsCount; i++){
            // Center to screen
            int xPos = graphicsHelper_GetPositionToCenterText(strlen(menu->options[i].text) + ((currentOption == i) * 2));
            graphicsHelper_CursorAt(xPos, i + 5);
            // Is this the selected option?
            if(currentOption == i) printf("[%s]\n", menu->options[i].text);
            else printf("%s\n", menu->options[i].text);
        }
    

        // Get input
        c = getchar();

        // What we do?
        switch(c){
            case 's':
                if(currentOption + 1 < menu->optionsCount) currentOption++;
                break;
            case 'w':
                if(currentOption - 1 > -1) currentOption--;
                break;
            case '\n':
                loop = 0;
                break;
            default:
                break;
        }
    }

    // Callback!
    menu->options[currentOption].callback();
}

// Free a menu
void graphicsFreeMenu(G_Menu* menu){
    free(menu->options);
    free(menu);
}

//
// GAME
//

// Init backdrop
void graphicsInitBackdrop(){
    graphicsHelper_SetColor(0, 0, 0);
    printf("\033[2J\033[H");
    graphicsHelper_CursorAt(gameXOffset, gameYOffset);
    for(int y = gameYOffset; y < gameYOffset + TETRIS_HEIGHT * 2; y++){
        graphicsHelper_SetColor(100, 100, 100);
        for(int x = gameXOffset; x < gameXOffset + TETRIS_WIDTH; x++){
            if(y % 2 != 0)
                printf(TETRIS_BACKGROUND);
            else printf("   ");
        }
        graphicsHelper_SetColor(0, 0, 0);
        graphicsHelper_CursorAt(gameXOffset, y);
    }
}

// Draw frame
void graphicsDrawFrame(G_Block currentBlock){
    graphicsInitBackdrop();
    // Draw the current block
    graphicsHelper_SetColor(currentBlock.shape.color.r, currentBlock.shape.color.b, currentBlock.shape.color.g);
    for(int i = 0; i < 4; i++){
        int xPos = gameXOffset + (currentBlock.pos.x * 3) + (currentBlock.shape.spaces[i].x * 3);
        int yPos = gameYOffset + currentBlock.pos.y - currentBlock.shape.spaces[i].y;
        if((yPos % 2)){
            // Make it thick
            graphicsHelper_CursorAt(xPos, yPos);
            printf("   ");
        }
        graphicsHelper_CursorAt(xPos, yPos);
        printf("   ");
    }
    fflush(stdout);
}


//
// HELPERS
// 

// What X-Pos to have centered text
int graphicsHelper_GetPositionToCenterText(int len){
    return (terminalWidth / 2) - (len / 2);
}

// Move cursor to X, Y position
void graphicsHelper_CursorAt(int x, int y){
    printf("\033[%d;%dH", y, x);
}

// Set terminal color
void graphicsHelper_SetColor(int r, int g, int b){
    printf("\033[48;2;%d;%d;%dm", r, g, b);
}
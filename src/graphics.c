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
    gameYOffset = (terminalHeight / 2) - (TETRIS_HEIGHT / 2);
    gameXOffset = (terminalWidth / 2) - ((strlen(TETRIS_BACKGROUND) * TETRIS_WIDTH) / 2);

    printf("Terminal Size: %d %d\nGame Pos: %d %d", terminalHeight, terminalWidth, gameXOffset, gameYOffset);

    // Setup input 
    // Hide Input
    struct termios termInfo;
    tcgetattr(0, &termInfo);
    termInfo.c_lflag &= ~ICANON;
    termInfo.c_cc[VMIN] = 1;
    termInfo.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &termInfo);
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
    printf("\033[2J\033[H");
    graphicsHelper_CursorAt(gameXOffset, gameYOffset);
    for(int y = gameYOffset; y < gameYOffset + TETRIS_HEIGHT; y++){
        for(int x = gameXOffset; x < gameXOffset + TETRIS_WIDTH; x++){
            printf(TETRIS_BACKGROUND);
        }
        graphicsHelper_CursorAt(gameXOffset, y);
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
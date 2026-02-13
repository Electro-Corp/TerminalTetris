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
    printf("\033[?25m");

    // Init the map
    for(int y = 0; y < TETRIS_HEIGHT; y++){
        for(int x = 0; x < TETRIS_WIDTH; x++){
            G_Tile tmp = {backgroundColor, 0};
            map[y][x] = tmp;
        }
    }

    // Setup input 
    // Hide Input
    struct termios termInfo;
    tcgetattr(0, &termInfo);
    termInfo.c_lflag &= ~ICANON;
    termInfo.c_lflag &= ~ECHO;
    termInfo.c_cc[VMIN] = 1;
    termInfo.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &termInfo);
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
    // for(int y = gameYOffset; y < gameYOffset + (TETRIS_HEIGHT * 2); y++){
    //     graphicsHelper_CursorAt(gameXOffset, y);
    //     for(int x = gameXOffset; x < gameXOffset + TETRIS_WIDTH; x++){
    //         G_Tile tile = map[((y - gameYOffset) / 2)][x - gameXOffset];
    //         graphicsHelper_SetColor(tile.color.r, tile.color.g, tile.color.b);
    //         printf("-%d-", tile.empty);
    //     }
    // }

    int mapX = 0, mapY = 0;

    for(int y = gameYOffset; y < gameYOffset + (TETRIS_HEIGHT * 2); y++){
        graphicsHelper_CursorAt(gameXOffset, y);
        for(int x = gameXOffset; x < gameXOffset + TETRIS_WIDTH; x++){
            G_Tile tile = map[mapY][x - gameXOffset];
            graphicsHelper_SetColor(tile.color.r, tile.color.g, tile.color.b);
            printf("   ", tile.empty);
        }
        if(y % 2 != 0) mapY++;
    }
}

// Draw frame
void graphicsDrawFrame(G_Block currentBlock){
    graphicsInitBackdrop();
    // Debug
    graphicsHelper_SetColor(0, 0, 0);
    graphicsHelper_CursorAt(gameXOffset, gameYOffset - 1);
    printf("Block Position: %d %d", blockGetExtremeOnBlock(currentBlock, 2).x + currentBlock.pos.x, blockGetExtremeOnBlock(currentBlock, 2).y + currentBlock.pos.y);
    // Draw the current block
    graphicsHelper_SetColor(currentBlock.shape.color.r, currentBlock.shape.color.g, currentBlock.shape.color.b);
    for(int i = 0; i < 4; i++){
        int xPos = gameXOffset + (currentBlock.pos.x * 3) + (currentBlock.shape.spaces[i].x * 3);
        int yPos = gameYOffset + currentBlock.pos.y * 2 + currentBlock.shape.spaces[i].y;
        if(!(yPos % 2)){
            // Make it thick
            graphicsHelper_CursorAt(xPos, yPos);
            printf("   ");
        }
        graphicsHelper_CursorAt(xPos, yPos);
        printf("   ");
    }
    graphicsDoWeClear();
    //fflush(stdout);
}   

// Add block to map
void graphicsAddBlockToMap(G_Block block){
    // This function does not care about anything
    // but pushing the block
    for(int i = 0; i < 4; i++){
        G_Tile tile;
        tile.color = block.shape.color;
        tile.empty = 1;
        int yPos = (block.pos.y - 1) + block.shape.spaces[i].y, xPos = block.pos.x + block.shape.spaces[i].x;
        map[yPos][xPos] = tile;
    }
}

// Check if square below is a block
int graphicsIsHittingOtherBlock(G_Block block){
    for(int i = 0; i < 4; i++){
        G_Position pos = block.shape.spaces[i];
        pos.x += block.pos.x;
        pos.y += block.pos.y;
        if(pos.y < TETRIS_HEIGHT){
            if(map[pos.y][pos.x].empty){
                return 1;
            }
        }
    }
    return 0;
}

// Check if we need to clear
void graphicsDoWeClear(){
    for(int y = 0; y < TETRIS_HEIGHT; y++){
        int clear = 1; // Clear or not? 
        for(int i = 0; i < TETRIS_WIDTH; i++){
            if(!map[y][i].empty) clear = 0; // no
        }
        // Did we pass?
        if(clear){
            graphicsClearRow(y);
        }
    }
}

// Clear bottom row
void graphicsClearRow(int row){
    for(int i = 0; i < TETRIS_WIDTH; i++){
        map[row][i].empty = 0;
        map[row][i].color = backgroundColor;
    }

    for(int i = row; i > 0; i--){
        memcpy(map[i], map[i - 1], PHYS_TETRIS_WIDTH);
    }
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

// Reset terminal pos and color
void graphicsHelper_ResetTerm(){
    printf("\033[2J\033[H\033[0m");
    printf("\033[5m");
}
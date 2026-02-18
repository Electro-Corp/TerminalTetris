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

    // Hide cursor
    printf("\033[?25l");

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

// Input a string
char* graphicsInputString(char* message, int size){
    // Clear screen
    graphicsHelper_SetColor(0, 0, 0);
    printf("\033[2J\033[H");
    // Print background
    graphicsHelper_SetColor(100, 100, 100);
    int xLen = graphicsHelper_GetPositionToCenterText(strlen(message));
    for(int y = gameYOffset + 5; y < gameYOffset + 7; y++){
        for(int x = xLen; x < xLen + strlen(message); x++){
            graphicsHelper_CursorAt(x, y);
            printf(" ");
        }
    }
    // Print message
    graphicsHelper_CursorAt(xLen, gameYOffset + 6);
    printf(message);
    // Get input
    graphicsHelper_CursorAt(xLen, gameYOffset + 7);
    char* input = malloc(sizeof(char) * size);
    fgets(input, size, stdin);
    input[strcspn(input, "\n")] = 0; 
    return input;
}

//
// GAME
//

// Init backdrop
void graphicsInitBackdrop(){
    graphicsHelper_SetColor(0, 0, 0);
    printf("\033[2J\033[H");
    // Game Title
    graphicsHelper_CursorAt(gameXOffset, gameYOffset - 1);
    printf("T  E  T  R  I  S");
    // Get ready
    graphicsHelper_CursorAt(gameXOffset, gameYOffset);

    int mapX = 0, mapY = 0;

    for(int y = gameYOffset; y < gameYOffset + (TETRIS_HEIGHT * HEIGHT_MULTIPLIER); y++){
        graphicsHelper_CursorAt(gameXOffset, y);
        for(int x = gameXOffset; x < gameXOffset + TETRIS_WIDTH; x++){
            G_Tile tile = map[mapY][x - gameXOffset];
            graphicsHelper_SetColor(tile.color.r, tile.color.g, tile.color.b);
            printf("   ", tile.empty);
        }
        mapY++;
        //if(y % HEIGHT_MULTIPLIER != 0) mapY++;
    }

    // Get next block
    G_Block next = blockGetComingUp();
    // Print title
    graphicsHelper_SetColor(0, 0, 0);
    graphicsHelper_CursorAt(gameXOffset + (TETRIS_WIDTH * 3) + 1, NEXT_BLOCK_VISUAL_Y + gameYOffset - 3);
    printf("Next block: ");
    // Draw next block
    graphicsHelper_SetColor(next.shape.color.r, next.shape.color.g, next.shape.color.b);
    for(int i = 0; i < 4; i++){
        graphicsHelper_CursorAt(NEXT_BLOCK_VISUAL_X + gameXOffset + (TETRIS_WIDTH * 3) + next.shape.spaces[i].x, NEXT_BLOCK_VISUAL_Y + gameYOffset + next.shape.spaces[i].y);
        printf("  ", next.pos.x);
    }
}

// Draw frame
void graphicsDrawFrame(G_Block currentBlock, int holding, double time){
    // Clear the last drawn pos
    graphicsHelper_SetColor(backgroundColor.r, backgroundColor.g, backgroundColor.b);
    for(int i = 0; i < 8; i++) {
        graphicsHelper_CursorAt(refill[i].x, refill[i].y);
        printf("   ");
    }

    // Draw the ghost block
    ghostBlock = currentBlock;
    while(!(blockGetExtremeOnBlock(ghostBlock, 2).y + ghostBlock.pos.y == TETRIS_HEIGHT || graphicsIsHittingOtherBlock(ghostBlock))){
        ghostBlock.pos.y++;
    }
    ghostBlock.pos.y--; // Go up one step
    graphicsHelper_SetColor(backgroundColor.r + 50, backgroundColor.g + 50, backgroundColor.b + 50);
    for(int i = 0; i < 4; i++){
        int xPos = gameXOffset + (ghostBlock.pos.x * 3) + (ghostBlock.shape.spaces[i].x * 3);
        int yPos = gameYOffset + ghostBlock.pos.y + ghostBlock.shape.spaces[i].y;
        graphicsHelper_CursorAt(xPos, yPos);
        printf("   ");
        // Save for next frame 
        refill[i + 4].x = xPos;
        refill[i + 4].y = yPos;
    }

    if(redrawTime) graphicsInitBackdrop();
        
    // Draw information
    graphicsHelper_SetColor(0, 0, 0);
    graphicsHelper_CursorAt(gameXOffset + (TETRIS_WIDTH * 3) + 2, gameYOffset + (TETRIS_HEIGHT * 0.75));
    printf("SCORE: %d", score);
    graphicsHelper_CursorAt(gameXOffset + (TETRIS_WIDTH * 3) + 2, gameYOffset + (TETRIS_HEIGHT * 0.75) + 2);
    printf("LEVEL: %d", level);
    graphicsHelper_CursorAt(gameXOffset + (TETRIS_WIDTH * 3) + 2, gameYOffset + (TETRIS_HEIGHT * 0.75) + 4);
    printf("CLEARED: %d", linesCleared);
    // Debug Block Position
    //graphicsHelper_CursorAt(gameXOffset + (TETRIS_WIDTH * 3) + 2, gameYOffset + (TETRIS_HEIGHT * 0.75) + 6);
    //printf("BLOCK POSITION: %d %d (%f)", currentBlock.pos.x, currentBlock.pos.y, time);
    
    //
    // Draw the current block
    //

    // Make it blink if we're holding at the bottom
    if(holding){
        graphicsHelper_SetColor(currentBlock.shape.color.r * time, currentBlock.shape.color.g * time, currentBlock.shape.color.b * time);
    }else{
        graphicsHelper_SetColor(currentBlock.shape.color.r, currentBlock.shape.color.g, currentBlock.shape.color.b);
    }
    for(int i = 0; i < 4; i++){
        int xPos = gameXOffset + (currentBlock.pos.x * 3) + (currentBlock.shape.spaces[i].x * 3);
        int yPos = gameYOffset + currentBlock.pos.y + currentBlock.shape.spaces[i].y - 1;
        if(!(yPos % 2)){
            // Make it thick
            graphicsHelper_CursorAt(xPos, yPos);
            printf("   ");
        }
        graphicsHelper_CursorAt(xPos, yPos);
        printf("   ");
        // Save for next frame 
        refill[i].x = xPos;
        refill[i].y = yPos;
    }
    redrawTime = 0;
    graphicsDoWeClear();
}   

// Draw pause
void graphicsDrawPause(){
    printf("\033[2J\033[H");
    graphicsHelper_SetColor(150, 150, 150);
    for(int y = gameYOffset + (TETRIS_HEIGHT / 2) - 2; y < gameYOffset + ((TETRIS_HEIGHT / 2) + 2); y++){
        for(int x = gameXOffset + 5; x < gameXOffset + 25; x++){
            graphicsHelper_CursorAt(x, y);
            printf(" ");
        }
    }
    graphicsHelper_SetColor(100, 100, 100);
    graphicsHelper_CursorAt(gameXOffset + 9, gameYOffset + (TETRIS_HEIGHT / 2));
    printf("P A U S E D");
    fflush(stdout);
}

// Draw game over
void graphicsDrawGameOver(HSCORE_ENTRY* scores){
    graphicsHelper_SetColor(205, 32, 32);
    for(int y = gameYOffset + 5; y < gameYOffset + ((TETRIS_HEIGHT * 0.75) + 2); y++){
        for(int x = gameXOffset + 5; x < gameXOffset + 25; x++){
            graphicsHelper_CursorAt(x, y);
            printf(" ");
        }
    }
    graphicsHelper_SetColor(255, 100, 100);
    graphicsHelper_CursorAt(gameXOffset + 6, gameYOffset + 6);
    printf("GAME OVER!");
    // Print High scores
    for(int i = 10; i > 0; i--){
        graphicsHelper_SetColor(205, 32, 32);
        graphicsHelper_CursorAt(gameXOffset + 6, gameYOffset + 8 + ((10 - i) * 2));
        printf("%s -> %d", (*(scores + i)).name, (*(scores + i)).score);
    }
    graphicsHelper_CursorAt(gameXOffset + 6, gameYOffset + 29);
    printf("\'q\' to continue!");
    fflush(stdout);
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
    // Redraw time 
    redrawTime = 1;
}

// Check if square below is a block
int graphicsIsHittingOtherBlock(G_Block block){
    for(int i = 0; i < 4; i++){
        if(graphicsSquareHittingBook(block.pos, block.shape.spaces[i], 2)){
            return 1;
        }
    }
    return 0;
}


// Check if a specfic square is boutta hit a block
int graphicsSquareHittingBook(G_Position root, G_Position pos, int dir){
    G_Position tmp = pos;
    tmp.x += root.x;
    tmp.y += root.y;
    switch(dir){
        // Left
        case 0:
            if(tmp.x > 0){
                if(map[tmp.y][tmp.x - 1].empty) return 1;
            }
            break;
        // Right
        case 1:
            if(tmp.x < TETRIS_WIDTH){
                if(map[tmp.y][tmp.x + 1].empty) return 1;
            }
            break;
        // Down
        case 2: 
            if(tmp.y < TETRIS_HEIGHT){
                if(map[tmp.y][tmp.x].empty) return 1;
            }
            break;
    }
    
    return 0;
}

// Check if we need to clear
void graphicsDoWeClear(){
    // How many cleared lines
    int lines = 0;
    // Check
    for(int y = 0; y < TETRIS_HEIGHT; y++){
        int clear = 1; // Clear or not? 
        for(int i = 0; i < TETRIS_WIDTH; i++){
            if(!map[y][i].empty) clear = 0; // no
        }
        // Did we pass?
        if(clear){
            redrawTime = 1;
            graphicsClearRow(y);
            lines++;
        }
    }
    // Update score
    switch(lines){
        case 0:
            // No lines cleared, don't do jack
            break;
        case 1:
            score += 40 * (level + 1);
            break;
        case 2:
            score += 100 * (level + 1);
            break;
        case 3:
            score += 300 * (level + 1);
            break;
        default:
        case 4:
            score += 1200 * (level + 1);
            break;
    }

    if(lines > 0){
        for(int i = 0; i < lines; i++){
            linesCleared++;
            if(linesCleared % 10 == 0) level++;
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

// Add to score
void graphicsAddToScore(int n){
    score += n;
}

int graphicsGetScore(){
    return score;
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
    printf("\033[?25h");
}
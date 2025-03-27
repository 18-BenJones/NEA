#include<SDL2/SDL.h>            // for SDL 
#include<stdlib.h>              // for rand()
#include<time.h>                // to seed rand() with srand(time(NULL))
#include<strings.h>             // for adjusting mindow text

// ------ Macros ------ 

#define HEIGHT 800              // these dont change at runtime, so this saves memory by making them
#define WIDTH HEIGHT - HEIGHT/2 // macros, as macros get replaced during the preproccessor
#define BLOCKLEN HEIGHT/20      // uses height as the constraint to get 20 equal blocks on the screen
#define FRAMERATE 60            // number of frames drawn per second

// ------ Structures ------ 

struct boardpos {                               // a struct used to encapsulate an x & y board position for logic and drawing blocks
    int x;              // x = 0 is the left
    int y;              // y = 0 is the top
};

struct st {                                     // aggregate all the data about the games state into a single struct        
    bool gamerun;       // stores if the game is running
    int score;          // track the score
    int rng;            // stores the type of rng in use
};

struct cur {                                    // Structure that encapsulates the position, type, rotation and colour of the current piece
    boardpos pos;       // position of the top left part of the matrix
    int piece[4][4];    // 2D array in the same form as shape[][][]'s that stores type, colour, and rotation 
    int held;           // tracks the index in shape[] of the held piece
};

// ------ global variables ------   

// global because they all need to be accessed from any scope, and for board[][] not decay into an *int[][]  
SDL_Window* window;                             // SDL object that contains the functionality of the window
SDL_Renderer* renderer;                         // SDL object that contains OpenGL functionality 
SDL_Event ev;                                   // SDL object that handles user input
int board[20][10] = {0};                        // a 2D matrix that represents the board. Any non-zero value is interpreted as a block
int piecequeue[7] = {-1,-1,-1,-1,-1,-1,-1};     // stores the indecies of the next 7 pieces to be played 
const Uint8 colour[7][3] {                      // defines an SDL_Color compatible array based of the value stored by the shape matrices and board[][] 
    {65, 230, 255},     // light blue
    {0, 0, 255},        // Dark blue,
    {255, 165, 65},     // orange
    {255, 255, 0},      // yellow
    {0, 255, 0 },       // green
    {210, 0, 255},      // purple
    {255, 0, 0}         // red
};
const int shape[7][4][4] = {                    // Stores all the shapes as a 3D array, using the values of the numbers to store colour
    {           // I
                {0,1,0,0},
                {0,1,0,0},
                {0,1,0,0},
                {0,1,0,0}
    },
    {           // J
                {0,0,2,0},
                {0,0,2,0},
                {0,2,2,0},
                {0,0,0,0}
    },
    {           // L
                {0,3,0,0},
                {0,3,0,0},
                {0,3,3,0},
                {0,0,0,0}
    },
    {           // O
                {0,4,4,0},
                {0,4,4,0},
                {0,0,0,0},
                {0,0,0,0}
    },
    {           // S
                {0,5,0,0},
                {0,5,5,0},
                {0,0,5,0},
                {0,0,0,0}
    },
    {           // T
                {0,6,0,0},
                {0,6,6,0},
                {0,6,0,0},
                {0,0,0,0}
    },
    {           // Z
                {0,0,7,0},
                {0,7,7,0},
                {0,7,0,0},
                {0,0,0,0}
    }
};
cur current = {3, 0,                            // Represents the current piece
   {{0,0,0,0},          // x = 3 is the 4th space; y = -1 because it moves down once on initialization to add it to board[][]        
    {0,0,0,0},          // init matrix that gets overridden by the first piece
    {0,0,0,0},
    {0,0,0,0}},
    -1
};
st state = { true, 0, 1};               // state variable to track the state of the application

// ------ Logic functions ------

void refreshqueue(){                            // manages the RNG and setting for the next pieces to be played
    int tmp = 0;
    int sum = 0;
    int indexes[7] = {0,1,2,3,4,5,6};

    for(int i = 0; i < 7; i++){// shift the list along  
        if(i==6)
            piecequeue[i] = -1;
        else{
            piecequeue[i] = piecequeue[i+1];
        } 
    }  
    if (state.rng == 0){        // true randomness
        for(int i = 0; i < 7; i++){
            if(piecequeue[i] == -1)
                piecequeue[i] = rand()%7;
        }
    }else{                      // 7 bag 
        for(int i = 0; i < 7; i++){
            sum = sum + piecequeue[i];  // use sum to store the sum
        }
        if (sum == -7){    // when the list is fully empty, need to reinitialise it
            sum = 0;// no longer need the sum so use it for the index of non -1 values
            while (sum < 7){ 
                tmp = rand()%7;
                if(indexes[tmp]!= -1){  // randomly select any non -1 index to create an inital order of the bag
                    piecequeue[sum] = indexes[tmp];
                    indexes[tmp] = -1;
                    sum++;
                } 
            }
        }else{
        return;
        }
    }
}

void transpose(int* fpntr, int len){            // Transposes the matrix (swaps elements about the diagonal centre line)
    int swap;
    for(int i = 0; i < len; i++){
        for(int j = i; j < len; j++){
            if(i==j) {continue;}        // ignore central line
            else{
                swap = *(fpntr + (len*j)+ i);
                *(fpntr + (len*j)+ i) = *(fpntr + (len*i)+ j);
                *(fpntr + (len*i)+ j) = swap;
            }
        }
    }
}

void reverse(int* fpntr, int len, bool ccw){    // Reverses either the rows or columns of a matrix based on ccw
    int swap;
    if(ccw == 1){                                               // Revese the columns
        if(len % 2 == 1){
            for(int i = 0; i < (len-1)/2; i++){                 // ignore middle row
                for(int j = 0; j < len; j++){
                    swap = *(fpntr+j+((len*len)-(len*(i+1))));  // for a 4x4: f(0) = 12, f(1) = 8, thus f(i) = l^2-l(i+1) 
                    *(fpntr+j+((len*len)-(len*(i+1)))) = *(fpntr+j+(len*i));
                    *(fpntr+j+(len*i)) = swap;
                }
            }
        }else{
            for(int i = 0; i < len/2; i++){                     // same as above but for even length
                for(int j = 0; j < len; j++){
                    swap = *(fpntr+j+((len*len)-(len*(i+1)))); 
                    *(fpntr+j+((len*len)-(len*(i+1)))) = *(fpntr+j+(len*i));
                    *(fpntr+j+(len*i)) = swap;
                }
            }
        }
    }
    else{                                                       // Reverse the rows
        if(len % 2 == 1){
            for(int i = 0; i < len; i++){
                for(int j = 0; j < ((len-1)/2)+1; j++){         // ignore middle column
                    swap = *(fpntr+(len-j-1)+(i*len));
                    *(fpntr+(len-j-1)+(i*len)) = *(fpntr+j+(len*i));
                    *(fpntr+j+(len*i)) = swap;
                }
            }
        }
        else{                                                   // same as above but for even length
            for(int i = 0; i < len; i++){
                for(int j = 0; j < (len)/2; j++){   
                    swap = *(fpntr+(len-j-1)+(i*len));
                    *(fpntr+(len-j-1)+(i*len)) = *(fpntr+j+(len*i));
                    *(fpntr+j+(len*i)) = swap;
                }    
            }
        }
    }
}

void rotate(int* fpntr, int len, bool ccw){     // rotates a matrix 90 deg clockwise or counterclocwise just using pointers
    // fpntr - front pointer, an int* to the first element [0][0]
    // len - length, the side length of the n x n matrix as an integer
    // ccw - counterclockwise, if 1 will rotate counterclockwise
    transpose(fpntr, len);
    reverse(fpntr, len, ccw);    
}

void changepiece(int action){                   // sets current.piece to a piece from shape[]

    int tmp = 0; 
    if (action == 0){     // use the next from the queue, the defualt path
        current.pos.x = 3;
        current.pos.y = 0; 
        for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            current.piece[i][j] = shape[piecequeue[0]][i][j];
        }
    }

    piecequeue[0] = -1; //mark the first piece as used for removal
    refreshqueue();
    
    }else{                 // change to the held piece
        if (current.held == -1){    // not holding any piece
            for(int i = 0; i < 4; i++){         //remove the old piece from the board
                for(int j = 0; j < 4; j++){
                    if(current.piece[i][j] != 0){
                        tmp =  current.piece[i][j];     //grab the value of the old piece to swap current.held
                        board[current.pos.y + i][current.pos.x + j] = 0;
                    }
                }
            }
            current.held = tmp - 1;     // put original piece into hold 
            changepiece(0);             // set a new piece from the queue
        }else{
            for(int i = 0; i < 4; i++){         //remove the old piece from the board
                for(int j = 0; j < 4; j++){
                    if(current.piece[i][j] != 0){
                        tmp =  current.piece[i][j];     //grab the value of the old piece to swap current.held
                        board[current.pos.y + i][current.pos.x + j] = 0;
                    }
                }
            }
            for (int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){// store the new piece for rendering next tick
                    current.piece[i][j] = shape[current.held][i][j];    
                    }
                }
            current.held = tmp - 1;         // swap the original piece back into the hold        
            current.pos.x = 3;              // set the new position
            current.pos.y = 0; 
            for(int i = 0; i < 4; i++){         //add new piece to the board
                for(int j = 0; j < 4; j++){
                    if(current.piece[i][j] != 0){
                        board[current.pos.y + i][current.pos.x + j] =   current.piece[i][j];
                    }
                }
            }    

        }
    }
}

bool wishpos(boardpos pos){                     // return true if the current piece can occupy the given position & rotation, otherwise false
    //defined inbetween other void functions because its used in functions below it 
    for(int i = 0; i < 4; i++){               
        for(int j = 0; j < 4; j++){
            if(current.piece[i][j] != 0){
                if (((pos.y + i + 1 > 20) or (pos.x + j + 1 > 10)) or (pos.x + j < 0)){  // out of bounds
                    return false;
                }
                if(board[pos.y + i][pos.x + j] != 0){ // intersect with another block
                    return false;
                }
            }
        }
    }
    return true; // valid position 
}

void lineclear(){                               // FIX ME checks if any lines need clearing and clears them and adds to the score
    int linescleared = 0;
    int zerocount = 10;
    for(int i = 20; i > 0; i--){    // greater than 0 skips the top line, which should always be left as zeros
        for(int j = 0; j < 10; j++){
            if(board[i][j] != 0){
                zerocount = zerocount - 1;
            }
        }
        if (zerocount == 0){
            for(int n = i; n > 0; n--){
                for(int m = 0; m < 10; m++){
                    board[n][m] = board[n-1][m];
                }
            }    
            linescleared = linescleared + 1;
            i = i+1;    // go back up a row in case we dropped a full row
        }
        zerocount = 10;
    }
    state.score = state.score + (linescleared*linescleared);
}

void mcpstatic(){                               // Function to glue pieces that have hit the bottom to the board 
    for(int i = 0; i < 4; i++){     
        for(int j = 0; j < 4; j++){
            if(current.piece[i][j] != 0){
                board[current.pos.y + i][current.pos.x + j] = current.piece[i][j]; // replace the old piece where it was
            }
        }
    }

    lineclear();        // As the board has a new static piece, need to check for lines          
    changepiece(0);    // renew the peice and the position of current
    
    if(wishpos(current.pos)){       // draw the new piece 
        for(int i = 0; i < 4; i++){     
            for(int j = 0; j < 4; j++){
                if(current.piece[i][j] != 0){
                    board[current.pos.y + i][current.pos.x + j] = current.piece[i][j];
                }
            }
        }
    }else{                          // no space to draw the new peice; game over!
        state.gamerun = false;
    }
}

void movepiece(int dir){                        // Main function for moving the current peice through board[][]
    
    int dist = 1; // used to track the distance dropped by the hard drop

    for(int i = 0; i < 4; i++){            // This is destructive so that the current piece doesn't affect wishpos() 
        for(int j = 0; j < 4; j++){
            if(current.piece[i][j] != 0){
                board[current.pos.y + i][current.pos.x + j] = 0;
            }
        }
    }

    switch(dir){
        case 0: // down
            if (wishpos({current.pos.x, current.pos.y + 1})){
            current.pos.y = current.pos.y + 1;
            }else{
                mcpstatic();  // this function needs to get out movepiece() early  
                return;       //  to avoid blocks persisting after hitting the bottom
            }
        break;
        case 1: // left
            if(wishpos({current.pos.x - 1, current.pos.y})){
                current.pos.x = current.pos.x - 1;
                break;
            }else{
                break;
            }
        case 2: // right
            if(wishpos({current.pos.x + 1, current.pos.y})){
                current.pos.x = current.pos.x + 1;
                break;
            }else{
                break;
            }
        case 3: // hard drop
            while(wishpos({current.pos.x, current.pos.y + dist})){
                dist = dist + 1;
            }
            current.pos.y = current.pos.y + dist - 1;
            mcpstatic();    // same as 'down'
            return;         // we need to get outa here
        case 4: // rotate
            rotate(&current.piece[0][0], 4, 0); // as rotate() operates on current.piece[][] rather than board[][], this is non-destructive
            if(wishpos(current.pos)){
                break;
            }else{  // this if looks inverted because we need to change current back if the position was invalid
                rotate(&current.piece[0][0], 4, 1); // rotate the matrix back 
            }
            break;
    }

    for(int i = 0; i < 4; i++){             // read the piece to the board
        for(int j = 0; j < 4; j++){
            if(current.piece[i][j] != 0){
                board[current.pos.y + i][current.pos.x + j] = current.piece[i][j];
            }
        }
    }
}

void input(SDL_Event ev){                       // Input handler
    if((SDL_QUIT == ev.type) || (SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode)) {
        state.gamerun = false;      // exit on escape 
        }else{                                  
        switch(ev.type){
            case SDL_KEYUP:                     // the release of a key, slightly counterintuitivley
                switch(ev.key.keysym.sym) {
                    case SDLK_DOWN:
                        movepiece(0);
                        break;
                    case SDLK_LEFT:
                        movepiece(1);
                        break;
                    case SDLK_RIGHT:
                        movepiece(2);
                        break;
                    case SDLK_UP:    // hard drop
                        movepiece(3);
                        break;
                    case SDLK_SPACE: // rotate
                        movepiece(4);
                        break;
                    case SDLK_RETURN: // swap piece
                        changepiece(1);
                    break;
                }
        }
    }
}

// ------ Drawing functions ------

boardpos cvrtss(boardpos ogpos){                // converts a boardpos into its screen space (pixel coordinate) equivilent 
    boardpos screenspace;
    screenspace.x = ogpos.x * BLOCKLEN;
    screenspace.y = ogpos.y * BLOCKLEN;
    return screenspace;
}

void drawblock_old(boardpos pos, SDL_Color col){// UNUSED; add a draw call for a block. simplification of below  function
    pos = cvrtss(pos);                                                          // find the pixel coords for the block 
    SDL_Rect square = {pos.x, pos.y, BLOCKLEN, BLOCKLEN};                       // defines the block to be drawn
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, SDL_ALPHA_OPAQUE);    // set the colour
    SDL_RenderFillRect(renderer, &square);                                      // draws a filled square of the given colour 
}

void drawblock(boardpos pos, SDL_Color col){    // Draw a nicer looking block
    int cb[3] = {col.r, col.g, col.b};  // base colour
    int cl[3] = {col.r, col.g, col.b};  // brighter
    int cd[3] = {col.r, col.g, col.b};  // darker
    pos = cvrtss(pos);                  // find the pixel coords for the block 
    for(int i = 0; i < 3; i++){         // make them brighter and darker
        if (cl[i] < 200) {cl[i] = cl[i]+75;}
        if  (cd[i] > 200){cd[i] = cd[i]-50;}
    }   // the shapes below draw a square, then top and left brighter, bottom right dark and the joining corners the same
    SDL_Rect square =   {pos.x, pos.y, BLOCKLEN, BLOCKLEN};                       
    SDL_Rect top =      {pos.x, pos.y, BLOCKLEN, BLOCKLEN/10};
    SDL_Rect bottom =   {pos.x, pos.y+(BLOCKLEN-BLOCKLEN/10), BLOCKLEN, BLOCKLEN/10};
    SDL_Rect left =     {pos.x, pos.y, BLOCKLEN/10, BLOCKLEN};
    SDL_Rect right =    {pos.x + (BLOCKLEN-BLOCKLEN/10), pos.y, BLOCKLEN/10, BLOCKLEN};
    SDL_Rect tr =       {pos.x + (BLOCKLEN-BLOCKLEN/10), pos.y, BLOCKLEN/10, BLOCKLEN/10};
    SDL_Rect bl =       {pos.x, pos.y+(BLOCKLEN-BLOCKLEN/10), BLOCKLEN/10, BLOCKLEN/10};
    SDL_SetRenderDrawColor(renderer, cb[0], cb[1], cb[2], SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &square);                                      
    SDL_SetRenderDrawColor(renderer, cd[0], cd[1], cd[2], SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &bottom);
    SDL_RenderFillRect(renderer, &right); 
    SDL_SetRenderDrawColor(renderer, cl[0], cl[1], cl[2], SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &top);
    SDL_RenderFillRect(renderer, &left);
    SDL_SetRenderDrawColor(renderer, cb[0], cb[1], cb[2], SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &tr);
    SDL_RenderFillRect(renderer, &bl);
}

void drawgrid(){                                // Draws a white grid to differentiate the board from the background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect grid = {0, 0, BLOCKLEN, BLOCKLEN};
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 10; j++){
        SDL_RenderDrawRect(renderer, &grid);
        grid.x += BLOCKLEN;
        }
        grid.y += BLOCKLEN;
        grid.x = 0;
    }
}

void drawboard(){                               // takes the current state of the board and adds it to the render queue
    SDL_Color tcol;
    boardpos tpos = {0, 0};
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 10; j++){
            if(board[i][j] != 0){
                tpos = {j, i};  // grab the position of the block, below gets its colour
                tcol = {colour[board[i][j]-1][0], colour[board[i][j]-1][1], colour[board[i][j]-1][2]};
                drawblock(tpos, tcol);
            }
        }
    }
}

void windowmessage(){                           // Display game information in the window's text box
    char text[64] = "";
    char tmp[32] = "";
    switch (current.held){
        case -1:
            sprintf(tmp, "None");
            break;
        case 0:
            sprintf(tmp, "I");
            break;
        case 1:
            sprintf(tmp, "J");
            break;
        case 2:
            sprintf(tmp, "L");
            break;
        case 3:
            sprintf(tmp, "O");
            break;
        case 4:
            sprintf(tmp, "S");
            break;
            case 5:
            sprintf(tmp, "T");
        break;
            case 6:
            sprintf(tmp, "Z");
        break;    
    }
    sprintf(text, "Score: %d | Held: %s", state.score, tmp);
    SDL_SetWindowTitle(window, text);
}

void draw(){                                    // Primary rendering function that handles adding all neccesry geometry to the renderer's queue
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
    SDL_RenderClear(renderer);      // Clear the renderer after each frame
    drawgrid();                     // grid first so it gets overdrawn by the blocks
    drawboard();                    // draw the all the blocks
    windowmessage();                // Set the window message
    SDL_RenderPresent(renderer);    // tell OpenGL to draw whats in the renders buffer
}

// ------ Core Game Loop ------

void game(){                                     // Encloses the game 
    // Game initilisation
    srand(time(NULL));                          // seed the random number generator
    int dtime = 0;                              // delta time, used to track realworld time as framerate =/= time
    float elapsedt = 0;                         // elapsed time, stores the time since the game has started

    refreshqueue();                             // refresh the piecequeue so that there's an available piece
    changepiece(0);                             // choose a piece for the first piece
    
    for(int i = 0; i < 4; i++){                 // read the initial piece to the board
        for(int j = 0; j < 4; j++){
            if(current.piece[i][j] != 0){
                board[current.pos.y + i][current.pos.x + j] = current.piece[i][j];
            }
        }
    }

    // Game loop
    while(state.gamerun){           
            if(SDL_PollEvent(&ev)){         
                input(ev);                      // jump to the input handler on a keypress for immediacy
            }                       
            draw();       

            if(elapsedt > 200){
                if(dtime >= 200){                   // max tick rate of 200 ms
                    movepiece(0);                   // apply gravity 
                    draw();                         // always update screen on ticks
                    elapsedt = elapsedt + dtime/1000;
                    dtime = 0;
                }else{
                    dtime = dtime + 1000/FRAMERATE; // add the frame delay to deltatime
                }
            }else{
                if(dtime >= 1000*((4)/(0.005*elapsedt+2)-1)){   // updates get closer together as elapsed time increases 

                    movepiece(0);                   // apply gravity 
                    draw();                         // always update screen on ticks
                    elapsedt = elapsedt + dtime/1000;
                    dtime = 0;
                }else{
                    dtime = dtime + 1000/FRAMERATE; // add the frame delay to deltatime
                }
            }
            SDL_Delay(1000/FRAMERATE);          // per frame interval in milisecs   
    }

    // Game over 
    return;
}

// ------ Main entrypoint ------

int main( int argc, char** argv ){
        
    // Init SDL
    SDL_Init( SDL_INIT_EVERYTHING );
    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
   
    // Draw a black screen to give the renderer something to present for frame 1
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);                   
    SDL_RenderPresent(renderer);                
   
    game(); // start the game

    // Clearing up after exiting
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return state.score;
}

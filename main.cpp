#include<SDL2/SDL.h>            // for SDL 
#include<stdlib.h>              // for rand()
#include<time.h>                // to seed rand() with srand(time(NULL))

/*
    TODO:
    Now: finish tetris:
    * finish wishpos()
    * add line clearing logic to update(5)
    * finish gamestate struct (implement scoring, menu, gamespeed)
    * add basic menu system 
        
    later:
    * either:
    * make the clone better (fancy blocks, sounds, upcoming pieces, settings, hold function, psuedorandomness)
    * or write the AI (still need pseudorandomness)
    * write up
*/

// ------ global variables ------ 

SDL_Window* window;                             // SDL object that contains the functionality of the window
SDL_Renderer* renderer;                         // SDL object that contains OpenGL functionality
bool running;                                   // Track the state of the core game loop, global bcus needs to change regardless of scope
int WIDTH = 800;
int HEIGHT = 600;
int BLOCKLEN = HEIGHT/20;                       // uses height as the constraint to get 20 equal blocks on the screen
int board[20][10] = {
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0} };

struct gamestate {                              // UNUSED aggrigate all the data about the games state into a single struct
    bool running;
    int ticklen;
    int score;
};

struct boardpos {                               // a struct used to encapsulate an x - y board position of various things
    int x;
    int y;
};

struct cur {                                    // Structure that encapsulates the position, type, rotation and colour of the current piece
    boardpos pos;
    int piece[4][4]; 
};

const Uint8 colour[7][3] {                      // gives a colour based of the value stored by the shape matricies 
    {65, 230, 255},     // light blue
    {0, 0, 255},        // Dark blue,
    {255, 165, 65},     // orange
    {255, 255, 0},      // yellow
    {0, 255, 0 },       // green
    {210, 0, 255},      // purple
    {255, 0, 0}         // red
};

const int shape[7][4][4] = {                    // Stores the shapes as a 3D array, using the values of the numbers to store colour
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

cur current = {3, -1,                           // defualt block spawn position, 3 is actually 4th space, -1 because it moves down once on spawn
   {{0,0,0,0},                  // init matrix that gets overridden by the first piece
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0}}
};

// ------ Logic functions ------

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

void rotate(int* fpntr, int len, bool ccw){     // rotates a matrix 90 deg clockwise or counterclocwise using pointers
    // fpntr - front pointer, an int* to the first element [0][0]
    // len - length, the side length of the n x n matrix as an integer
    // ccw - counterclockwise, if 1 will rotate counterclockwise
    transpose(fpntr, len);
    reverse(fpntr, len, ccw);    
}

void changepeice(){                             // sets current.piece to a random piece from shape[]
    int rannum = rand()%7;                      // get a random number between 0 and 6 for index
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            current.piece[i][j] = shape[rannum][i][j];
        }
    }
}

bool wishpos(boardpos pos){                     // return true if the desired position is valid. defined here because its used in void functions below it 
    
}

void mcpstatic(){                               // Function to glue pieces that have hit the bottom to the board 
    for(int i = 0; i < 4; i++){     
        for(int j = 0; j < 4; j++){
            if(current.piece[i][j] != 0){
                board[current.pos.y + i][current.pos.x + j] = current.piece[i][j]; // replace the old piece where it was
            }
        }
    }

    current.pos.x = 3;
    current.pos.y = 0;  // reset all of current         
    changepeice();      // get a new piece 
    
    if(wishpos(current.pos)){       // draw the new piece 
        for(int i = 0; i < 4; i++){     
            for(int j = 0; j < 4; j++){
                if(current.piece[i][j] != 0){
                    board[current.pos.y + i][current.pos.x + j] = current.piece[i][j];
                }
            }
        }
    }else{                          // no space to draw the new peice; game over!
        running = false;
    }
}

void movepiece(int dir){                        // Main function for moving the current peice through board[][]
    
    int dist = 1; // used to track the distance dropped by the hard drop

    for(int i = 0; i < 4; i++){                        // This is destructive so that the current piece doesn't affect wishpos() 
        if(current.pos.y + i < 0){continue;}           // case for current.pos.y = -1
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
                mcpstatic();  // this is the only function that 
                return;       // get out movepiece() early as not to draw 2 peices      
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
            current.pos.y = current.pos.y + dist;
            break;
        case 4: // rotate
            rotate(&current.piece[0][0], 4, 0); // as rotate() operates on current.piece[][] rather than board[][], this is non-destructive
            if(wishpos(current.pos)){
                break;
            }else{  // this if looks inverted because we need to change current back if the position was invalid
                rotate(&current.piece[0][0], 4, 1); // rotate the matrix back 
            }
            break;
    }

    for(int i = 0; i < 4; i++){             // readd the piece to the board
        for(int j = 0; j < 4; j++){
            if(current.piece[i][j] != 0){
                board[current.pos.y + i][current.pos.x + j] = current.piece[i][j];
            }
        }
    }
}

void lineclear(){                               // UNFINISHED
    int zerocount = 10;
    int swap;
    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 10; j++){
            if (board[i][j] != 0){
                zerocount = zerocount - 1;
            }
            if(zerocount == 0){
                for(int k = 0; k < 10; k++){
                    board[0][k] = 0;    //set the top line back to zero
                }
                for(int n = 1; n < (i + 1); n++){
                    for(int m = 0; m < 10; m++){    // should go from bottom to top bcus its easier
                        swap = board[n][m];
                        board[n][m] = board[n-1][m];
                    }
                }
        zerocount = 10;
    }
}
}
}

boardpos cvrtss(boardpos ogpos){                // converts a boardpos into its screen space (pixel coordinate) equivilent 
    boardpos screenspace;
    screenspace.x = ogpos.x * BLOCKLEN;
    screenspace.y = ogpos.y * BLOCKLEN;
    return screenspace;
}

// ------ Drawing functions ------

void drawblock(boardpos pos, SDL_Color col){    // add a draw call to the render queue for a single block on the grid at a given board coordinate
    pos = cvrtss(pos);                                                          // find the pixel coords for the block
    SDL_Rect square = {pos.x, pos.y, BLOCKLEN, BLOCKLEN};                       // defines the block to be drawn
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, SDL_ALPHA_OPAQUE);    // set the colour
    SDL_RenderFillRect(renderer, &square);                                      // draws a filled square of the given colour 
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

// ------ Game loop Functions ------

void update(int action){                        // primary enclosing function for preforming game logic 
    // TODO finish this
    switch(action){
        case 0: // gravity
                movepiece(0);  
            break;
        case 1: // left
                movepiece(1);
            break;
        case 2: // right 
                movepiece(2);
            break;
        case 3: // down by 1
                movepiece(0);   // gravity is the same as going down; abstraction
        case 4: // hard drop
                movepiece(3);
            break;
        case 5: // rotate the piece
            movepiece(4); 
            break;
        case 6: // check for line clears
            lineclear();
            break;
    }
}

void input(SDL_Event ev){                       // Handle keyboard input via SDL
    if((SDL_QUIT == ev.type) || (SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode)) {
    running = false;                        // exit on escape 
    }else{                                  
    switch(ev.type){
        case SDL_KEYUP:                     // the release of a key, slightly counterintuitivley
            switch(ev.key.keysym.sym) {
                case SDLK_LEFT:
                    update(1);
                    break;
                case SDLK_RIGHT:
                    update(2);
                    break;
                case SDLK_DOWN:
                    update(3);
                    break;
                case SDLK_UP:    // hard drop
                    update(4);
                    break;
                case SDLK_SPACE: // rotate
                    update(5);
            }
        }
    }
}

void draw(){                                    // Primary rendering function that handles adding all neccesry geometry to the renderer's queue
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
    SDL_RenderClear(renderer);      // Clear the renderer after each frame
    drawgrid();                     // grid first so it gets overdrawn by the blocks
    drawboard();                    // draw the all the blocks
    SDL_RenderPresent(renderer);    // tell OpenGL to draw whats in the renders buffer
}

// ------ Main entrypoint ------

int main( int argc, char** argv ){
    // Init SDL
    SDL_Init( SDL_INIT_EVERYTHING );
    window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // Black screen for 1st frame in order to not redraw whats behind the window upon startup
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Game loop
    int framerate = 60; //these two are scoped here because only main() needs it
    SDL_Event ev;
    running = true;     

    int dtime = 0;
    
    srand(time(NULL));
    changepeice(); // initialise the first piece
    movepiece(0);  // put the piece into board[][] and move it down 1

    while( running )
    { 
            if(SDL_PollEvent(&ev)){
                input(ev);}          // jump to the input handler on a keypress 
            draw();
            if(dtime >= 1000){
                update(0);
                draw();              // always update screen on ticks
                dtime = 0;
            }else{
                dtime = dtime + 1000/framerate;
            }
            SDL_Delay(1000/framerate);  // per frame interval in milisecs
            
    }

    // Clearing up after exiting
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


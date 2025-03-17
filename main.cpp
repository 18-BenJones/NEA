#include<SDL2/SDL.h>
#include<vector>
#include<time.h>

// ------ global variables ------ 

SDL_Window* window;             // SDL object that contains the functionality of the window
SDL_Renderer* renderer;         // SDL object that contains OpenGL functionality
bool running;                   // Track the state of the core game loop
int WIDTH = 800;
int HEIGHT = 600;
int board[20][10];

struct boardpos {
    int x;
    int y;
};

struct cur {
    boardpos pos;
    int piece[4][4];
    SDL_Color col; 
};

const Uint8 colour[7][3] {                   // gives a colour based of the value stored by the shape matricies 
    {173, 216, 230},    // light blue
    {0, 0, 255},        // Dark blue,
    {255, 165, 0},      // orange
    {255, 165, 0},      // yellow
    {0, 255, 0 },       //green
    {255, 0, 255},      //purple
    {255, 0, 0}         //red
};

const int shape[7][4][4] = {    // Stores the shapes as a 3D array, using the values of the numbers to store colour
    {           // I
                {1,1,1,1},
                {0,0,0,0},
                {0,0,0,0},
                {0,0,0,0}
    },
    {           // J
                {2,0,0,0},
                {2,2,2,0},
                {0,0,0,0},
                {0,0,0,0}
    },
    {           // L
                {0,0,3,0},
                {3,3,3,0},
                {0,0,0,0},
                {0,0,0,0}
    },
    {           // O
                {4,4,0,0},
                {4,4,0,0},
                {0,0,0,0},
                {0,0,0,0}
    },
    {           // S
                {0,5,5,0},
                {5,5,0,0},
                {0,0,0,0},
                {0,0,0,0}
    },
    {           // T
                {0,6,0,0},
                {6,6,6,0},
                {0,0,0,0},
                {0,0,0,0}
    },
    {           // Z
                {7,7,0,0},
                {0,7,7,0},
                {0,0,0,0},
                {0,0,0,0}
    }
};

// ------ functions ------

int msleep(int msec){   // Sleep for the requested number of milliseconds.   
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = msec*1000000;
    nanosleep(&tim , &tim2);
}

void drawblock(boardpos cpos, SDL_Color col){
    // This shit broken asf
    //const SDL_Rect sq = {cpos.x, cpos.y, (20), (20)};
    const SDL_Rect sq = {50, 50, 200, 200};
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, 255);
    SDL_RenderDrawRect(renderer, &sq);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void transpose(int* fpntr, int len){
    // Transposes the matrix (swaps elements about the centre)
    int swap;
    for(int i = 0; i < len; i++){
        for(int j = i; j < len; j++){
            if(i==j) {
                continue;                                       // ignore central line
            }
            else{
                swap = *(fpntr + (len*j)+ i);
                *(fpntr + (len*j)+ i) = *(fpntr + (len*i)+ j);
                *(fpntr + (len*i)+ j) = swap;
            }
        }
    }
}

void reverse(int* fpntr, int len, bool ccw){
    // Reverses either the rows or columns of a matrix
    int swap;
    if(ccw == 1){                                               // Revese the columns
        if(len % 2 == 1){
            for(int i = 0; i < (len-1)/2; i++){                 // ignore middle row
                for(int j = 0; j < len; j++){
                    swap = *(fpntr+j+((len*len)-(len*(i+1))));  //for a 4x4: f(0) = 12, f(1) = 8, thus f(i) = l^2-l(i+1) 
                    *(fpntr+j+((len*len)-(len*(i+1)))) = *(fpntr+j+(len*i));
                    *(fpntr+j+(len*i)) = swap;
                }
            }
        }else{
            for(int i = 0; i < len/2; i++){
                for(int j = 0; j < len; j++){
                    swap = *(fpntr+j+((len*len)-(len*(i+1))));  // same as above
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
        else{
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

void rotate(int* fpntr, int len, bool ccw){
    // fpntr - front pointer, an int* to the first element [0][0]
    // len - length, the side length of the n x n matrix as an integer
    // ccw - counterclockwise, if 1 will rotate counterclockwise
    transpose(fpntr, len);
    reverse(fpntr, len, ccw);    
}

// ------ Game loop Functions ------

int input(SDL_Event ev){                    // Handle keyboard input via SDL
    if((SDL_QUIT == ev.type) || (SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode)) 
        {running = false;}      // exit on escape
    else{
        switch(ev.type){
            case SDL_KEYUP:     // the release of a key
                switch(ev.key.keysym.sym) {
                    case SDLK_LEFT:
                        return(1);
                    case SDLK_RIGHT:
                        return(2);
                    case SDLK_DOWN:
                        return(3);
                    case SDLK_UP:
                        return(4);
                    //case SDLK_ESCAPE:
                    //    running=false;
                default:
                    return(0);
                }
            default:
                return(0);
            }
        }
}

void update(int action){
    // preform the game calculations, eg clearing lines moving the current piece etc
    // this shit is also broken af
    switch(action){
        case 0: // apply gravity
        case 1: // left
        case 2: // right
        case 3: // hard drop
        case 4: // rotate 
        case 5: // line clear
    }
}

void render(){ // this needs to be fixed bcus of drawblock to add functionality
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        //draw the board
        //draw the current piece
        boardpos tmp = {50, 50};
        SDL_Color tmpc = {255, 255, 255};
        drawblock(tmp, tmpc);
        SDL_RenderPresent(renderer);
}

// ------ Main entrypoint ------

int main( int argc, char** argv )
{
    // Init SDL
    SDL_Init( SDL_INIT_EVERYTHING );
    window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Game loop
    int elapsedsec = 0;
    int ticklen = 20;
    SDL_Event ev;
    running = true;
    while( running )
    {
        for(int i = 0; i < ticklen + 1; i++){
            if(SDL_PollEvent(&ev)){
                update(input(ev));
            }
            msleep(1000/ticklen);
        }
        elapsedsec++;
        update(0);
        update(5);
        render();
    }
    
    // Clearing up after exiting
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


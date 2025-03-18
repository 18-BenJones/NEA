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
};

cur current = {0, 5,};

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

// ------ Matrix functions ------

void transpose(int* fpntr, int len){
    // Transposes the matrix (swaps elements about the centre)
    int swap;
    for(int i = 0; i < len; i++){
        for(int j = i; j < len; j++){
            if(i==j) {continue;}                                       // ignore central line
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

int distbelow(){
    // Calculate the available distance below a block
    return 1;
}

boardpos lowestpos(){
    // find the lowest part of the matrix
    boardpos ret = {0, 0};
    return ret;
}

// ------ Drawing functions ------

void drawblock(boardpos cpos, SDL_Color col){
    // This shit broken asf
    //const SDL_Rect sq = {cpos.x, cpos.y, (WIDTH/20), (WIDTH/20)};
    //const SDL_Rect sq = {50, 50, 200, 200};
}

void tt(){
    const std::vector< SDL_Vertex > verts =
    {
        { SDL_FPoint{ 400, 150 }, SDL_Color{ 255, 0, 0, 255 }, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ 200, 450 }, SDL_Color{ 0, 0, 255, 255 }, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ 600, 450 }, SDL_Color{ 0, 255, 0, 255 }, SDL_FPoint{ 0 }, },
    };
    SDL_RenderGeometry( renderer, nullptr, verts.data(), verts.size(), nullptr, 0 );
}

// ------ Game loop Functions ------

void update(int action){
    // preform the game calculations, eg clearing lines moving the current piece etc
    // this shit is also broken af
    
    switch(action){
        case 0: // apply gravity
            if (distbelow() >= 1){
            current.pos.y++;
            }
            break;
            case 1: // left
            current.pos.x--;
            break;
        case 2: // right
            current.pos.x++;
            break;
        case 3: // hard drop
            lowestpos(); 
            break;
        case 4: // rotate
            rotate(&current.piece[0][0], 4, 0); 
            break;
        case 5: // line clear
            // implement me later
            break; 
        default:
            //do nothing
            break;
    }
}
void input(SDL_Event ev){                    // Handle keyboard input via SDL
    if((SDL_QUIT == ev.type) || (SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode)) 
    {running = false;}      // exit on escape
    else{
    switch(ev.type){
        case SDL_KEYUP:     // the release of a key
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
                case SDLK_UP:
                    update(4);
                    break;
                //case SDLK_ESCAPE:
                //    running=false;
            default:
                return;
            }
        default:
            return;
        }
    }
}

void draw(){ // this needs to be fixed bcus of drawblock to add functionality
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        //boardpos tmp = {50, 50};
        //SDL_Color tmpc = {255, 255, 255, 255};
        //drawblock(tmp, tmpc);
        tt();
        SDL_RenderPresent(renderer);
}

// ------ Main entrypoint ------

int main( int argc, char** argv )
{
    // Init SDL
    SDL_Init( SDL_INIT_EVERYTHING );
    window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    ///draw black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Game loop
    int framerate = 60;
    int ticklen = 20;
    SDL_Event ev;
    running = true;
    ticklen = 100;

    while( running )
    { 
        //for(int i = 0; i < ticklen + 1; i++){
            if(SDL_PollEvent(&ev))
            {input(ev);}
            update(0);
            update(5);
            draw();
            SDL_Delay(1000/framerate);
    }

    // Clearing up after exiting
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


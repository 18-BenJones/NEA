#include<SDL2/SDL.h>
#include<vector>

// ------ global variables ------ 

SDL_Window* window;             // SDL object that contains the functionality of the window
SDL_Renderer* renderer;         // SDL object that contains OpenGL functionality
bool running;                   // Track the state of the core game loop
int WIDTH = 800;
int HEIGHT = 600;
int board[20][10];

enum colour {                   // gives a colour based of the value stored by the shape matricies 
    lblue,
    dblue,
    orange,
    yellow,
    green,
    purple,
    red
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

// ------ Game loop Functions ------

void wait(){

}

int input(){                    // Handle keyboard input via SDL
    SDL_Event ev;
    SDL_PollEvent(&ev);
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
                    case SDLK_ESCAPE:
                        running=false;
                }
            }
        }
}

void update(){
    // preform the game calculations, eg clearing lines or 
}

void render(){
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        //draw the board
        //draw the current piece
        SDL_RenderPresent(renderer);
}

// ------ Specific functions ------

void rotate(){  // rotate the current piece

}

void lineclear(){

}

// ------ Main entrypoint ------

int main( int argc, char** argv )
{
    // Init SDL
    SDL_Init( SDL_INIT_EVERYTHING );
    window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Game loop
    running = true;
    while( running )
    {
        wait();
        input();
        update();
        render();
    }

    // Clearing up after exiting
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();
    return 0;
}


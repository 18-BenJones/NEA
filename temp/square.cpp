#include<SDL2/SDL.h>
#include<vector>

SDL_Window* window;
SDL_Renderer* renderer;
bool running;                   
int WIDTH = 800;
int HEIGHT = 600;

void square(){
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_Rect square = { 300, 200, 100, 100 };
    SDL_RenderFillRect(renderer, &square);
}

void draw(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    square();
    SDL_RenderPresent(renderer);
}

int main( int argc, char** argv )
{
    // Init SDL
    SDL_Init( SDL_INIT_EVERYTHING );
    window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // Black screen for 1st frame in order to not redraw whats behind the window upon startup
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Game loop
    int framerate = 60;
    int ticklen = 20;
    SDL_Event ev;
    running = true;

    while( running )
    {   // quit condition
        if(SDL_PollEvent(&ev)){if((SDL_QUIT == ev.type) || (SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode)) {running = false;}}
            draw();
            SDL_Delay(1000/framerate);
    }

    // Clearing up after exiting
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
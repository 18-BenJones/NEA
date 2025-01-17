#include<SDL2/SDL.h>
#include<vector>
#define WIDTH 800
#define HEIGHT 600

enum piece {
    I, 
    O, 
    T, 
    S, 
    Z, 
    J, 
    L
};

struct pip {
    int x; // position starts from top left at 0,0
    int y;
    enum piece p;
};

int board[20][10]; 

void draw_square(int x, int y)

int main( int argc, char** argv )
{
    //Driver code
    SDL_Init( SDL_INIT_EVERYTHING );
    SDL_Window* window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN );
    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

    SDL_Rect tr;
    tr.x = 10;
    tr.y = 10;
    tr.w = 100;
    tr.h = 100;

    	
    SDL_Surface * image = SDL_LoadBMP("image.bmp");
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, image);


    bool running = true;
    while( running )
    {
        SDL_Event ev;
        while( SDL_PollEvent( &ev ) )
        {
            if( ( SDL_QUIT == ev.type ) ||
                ( SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode ) ) // exit on escape
            {running = false;  break; }        }

    


       SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
       SDL_RenderClear(renderer);
       //SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE );
       //SDL_RenderDrawRect(renderer, &tr);
       SDL_RenderCopy(renderer, texture, NULL, NULL);
       
       SDL_RenderPresent(renderer);
       SDL_Delay(16);
       //tr.x += 1;
    }
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}


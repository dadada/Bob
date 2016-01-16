#include <iostream>
#include <utility>
#include <SDL2/SDL.h>
#include "GameMap.hpp"

#ifndef DEFAULTS
const Sint16 GRID_SIZE = 20;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGTH = 600;
#endif

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
Uint32 rmask = 0xff000000;
Uint32 gmask = 0x00ff0000;
Uint32 bmask = 0x0000ff00;
Uint32 amask = 0x000000ff;
#else
Uint32 rmask = 0x000000ff;
Uint32 gmask = 0x0000ff00;
Uint32 bmask = 0x00ff0000;
Uint32 amask = 0xff000000;
#endif

#ifndef Timer

class Timer
{
private:
    Uint32 timer_started;
public:
    void start_timer();

    Uint32 get_timer();

    Uint32 reset_timer();
};

#endif

#ifndef  Game
class Game
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    HexagonGrid *grid;
    bool move[4];
    bool full_screen;
    bool quit;
public:
    Game(SDL_Window *window_, SDL_Renderer *renderer_)
            : window(window_), renderer(renderer_)
    {
        Layout layout = {pointy_orientation, GRID_SIZE, {SCREEN_WIDTH / 2, SCREEN_HEIGTH / 2}};
        this->grid = new HexagonGrid(GRID_SIZE, layout);
        for (int i = 0; i < 4; i++)
        {
            this->move[i] = false;
        }
        this->quit = false;
    }

    void toggle_fullscreen();

    ~Game()
    {
        delete this->grid;
    }

    void handle_event(SDL_Event *event);
    int game_loop();
};

#endif

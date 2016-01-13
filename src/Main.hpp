#include <iostream>
#include <utility>
#include <SDL2/SDL.h>
#include "GameMap.hpp"

#ifndef DEFAULTS
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const SDL_Point CENTER = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
const Layout LAYOUT = Layout(pointy_orientation, 20.0, CENTER);
const Sint16 GRID_SIZE = 6;
const SDL_Color COLOR = {0, 255, 255, 255};
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


//template <class T>
class Game
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    HexagonGrid *grid;
public:
    Game(SDL_Window *window_, SDL_Renderer *renderer_)
            : window(window_), renderer(renderer_)
    {
        this->grid = new HexagonGrid(GRID_SIZE, LAYOUT, COLOR);
    }

    ~Game()
    {
        delete this->grid;
    }

    int game_loop();
};

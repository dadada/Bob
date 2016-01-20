#include <iostream>
#include <string>
#include <utility>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include "Gameplay.hpp"
#include <unordered_set>

#ifndef DEFAULTS
const Sint16 GRID_SIZE = 4;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGTH = 600;
const int SIDEBOX_WIDTH = 200;
const char TITLE[] = "Bob - Battles of Bacteria";
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
    // only deal with meta information
    Cluster fields_meta;
    Layout *layout;
    bool move[4];
    bool full_screen;
    bool quit;

public:
    Game(SDL_Window *window_, SDL_Renderer *renderer_, Layout *layout_)
            : window(window_), renderer(renderer_), layout(layout_)
    {
        this->grid = new HexagonGrid(GRID_SIZE, layout);
        for (int i = 0; i < 4; i++)
        {
            this->move[i] = false;
        }
        this->quit = false;
        // create meta information for every field on the grid
        for (const Field &elem : *(grid->get_fields()))
        {
            FieldMeta *meta = new FieldMeta(elem);
            fields_meta.insert(meta);
        }
    }

    ~Game()
    {
        for (const FieldMeta *elem : this->fields_meta)
        {
            delete elem;
        }
        delete this->grid;
    }

    void render(SDL_Renderer *renderer);

    void toggle_fullscreen();

    void handle_event(SDL_Event *event);

    int game_loop();
};

#endif

bool position_in_window(SDL_Point position, SDL_Window *window);

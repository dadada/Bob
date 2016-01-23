#include <iostream>
#include <string>
#include <utility>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <unordered_set>
#include "Exceptions.hpp"
#include "Gameplay.hpp"
//#include "Events.hpp"
#include "Gui.hpp"

#ifndef _BOB_H
#define _BOB_H

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGTH = 600;
const int SIDEBAR_WIDTH = 200;
const std::string TITLE = "Bob - Battles of Bacteria";

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

class Game
{

public:
    Game(SDL_Rect *window_dimensions, Sint16 size)
    {
        this->layout = new Layout(pointy_orientation, size,
                                  {window_dimensions->w / 2, window_dimensions->h / 2},
                                  {0, 0, window_dimensions->w - SIDEBAR_WIDTH, window_dimensions->h});
        this->grid = new HexagonGrid(size, this->layout);
        for (int i = 0; i < 4; i++)
        {
            this->move[i] = false;
        }
        this->quit = false;
        try
        {
            this->window = new Window(TITLE, window_dimensions, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
            this->renderer = new Renderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            SDL_Rect side_bar_dimensions = {window_dimensions->x - SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH,
                                            window_dimensions->y};
        }
        catch (const SDL_Exception &sdl_except)
        {
            std::cerr << sdl_except.what() << " happened when constructing game" << std::endl;
        }
        this->frame_timer = new Timer();
        this->move_timer = new Timer();
    }

    ~Game()
    {
        delete this->move_timer;
        delete this->frame_timer;
        //       delete this->side_bar;
        delete this->grid;
        delete this->renderer;
        delete this->window;
        delete this->layout;
    }

    void render();

    void handle_event(SDL_Event *event);

    int game_loop();

private:
    Window *window;
    Renderer *renderer;
    HexagonGrid *grid;
    Layout *layout;
    //   SideBar *side_bar;
    bool move[4];
    bool quit;
    Timer *frame_timer;
    Timer *move_timer;
};

#endif

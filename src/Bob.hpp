#ifndef _BOB_H
#define _BOB_H

#include <iostream>
#include <string>
#include <utility>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <unordered_set>
#include "Exceptions.hpp"
#include "Gameplay.hpp"
#include "Events.hpp"
#include "Gui.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGTH = 600;
const int SIDEBAR_WIDTH = 200;
const std::string TITLE = "Bob - Battles of Bacteria";

class Game
{

public:
    Game(SDL_Rect *window_dimensions, Sint16 size)
    {
        this->event_context = new EventContext();
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

            this->font = load_font_from_file("/usr/share/fonts/dejavu/DejaVuSans.ttf", 12);
            this->window = new Window(TITLE, window_dimensions, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
            this->renderer = new Renderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
                                                            | SDL_RENDERER_TARGETTEXTURE);
            SDL_Rect side_bar_dimensions = {window_dimensions->x - SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH,
                                            window_dimensions->y};
            this->side_bar = new Container(this->window, this->renderer, side_bar_dimensions);
            this->test_box = new TextBox(this->renderer,
                                         {SCREEN_WIDTH - SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, SCREEN_HEIGTH},
                                         {0x00, 0xff, 0x00, 0xff}, this->font);
            this->side_bar->add(test_box);
            this->side_bar->set_visible(true);
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
        delete this->test_box;
        delete this->move_timer;
        delete this->frame_timer;
        delete this->side_bar;
        delete this->grid;
        delete this->renderer;
        delete this->window;
        delete this->layout;
        delete this->event_context;
    }

    void render();

    void handle_event(SDL_Event *event);

    int game_loop();

private:
    EventContext *event_context;
    TextBox *test_box;
    TTF_Font *font;
    Window *window;
    Renderer *renderer;
    HexagonGrid *grid;
    Layout *layout;
    Container *side_bar;
    bool move[4];
    bool quit;
    Timer *frame_timer;
    Timer *move_timer;
};

#endif

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

const std::string TITLE = "Bob - Battles of Bacteria";

class Game
{

public:
    Game(SDL_Rect *window_dimensions, Sint16 size)
    {
        this->layout = new Layout(pointy_orientation, 20,
                                  {window_dimensions->w / 2, window_dimensions->h / 2},
                                  {0, 0, window_dimensions->w, window_dimensions->h});
        this->grid = new HexagonGrid(size, this->layout);
        for (int i = 0; i < 4; i++)
        {
            this->move[i] = false;
        }
        this->quit = false;
        SDL_Color fg = {0x00, 0x00, 0x00, 0xff};
        try
        {
            this->font = load_font_from_file("/usr/share/fonts/dejavu/DejaVuSans.ttf", 12);
            this->window = new Window(TITLE, window_dimensions, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
            this->renderer = new Renderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
                                                            | SDL_RENDERER_TARGETTEXTURE);
            FieldMeta *center = this->grid->get_field({0, 0, 0});
            this->field_box = new FieldBox(this->renderer, {0, 0, 1, 1}, fg, this->font, center);
            this->upgrade_box = new UpgradeBox(this->renderer, {0, 0, 1, 1}, fg, this->font, center);
            this->test_box = new TextBox(this->renderer, {0, 0, 1, 1}, fg, this->font);
            this->test_box->set_visible(true);
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
        delete this->upgrade_box;
        delete this->field_box;
        delete this->move_timer;
        delete this->frame_timer;
        delete this->grid;
        delete this->renderer;
        delete this->window;
        delete this->layout;
    }

    void render();

    void handle_event(SDL_Event *event);

    int game_loop();

private:
    UpgradeBox *upgrade_box;
    FieldBox *field_box;
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

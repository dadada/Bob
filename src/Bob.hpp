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
        this->adding = nullptr;
        this->started = false;
        this->layout = new Layout(pointy_orientation, 20,
                                  {window_dimensions->w / 2, window_dimensions->h / 2},
                                  {0, 0, window_dimensions->w, window_dimensions->h});
        for (int i = 0; i < 4; i++)
        {
            this->move[i] = false;
        }
        this->quit = false;
        SDL_Color fg = {0x00, 0x00, 0x00, 0xff};
        this->players = std::vector<Player *>();
        Player *default_player = new Player();
        this->players.push_back(default_player);
        try
        {
            this->font = load_font_from_file("/usr/share/fonts/dejavu/DejaVuSans.ttf", 20);
            this->window = new Window(TITLE, window_dimensions, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
            SDL_Point window_size = this->window->get_size();
            this->renderer = new Renderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
                                                            | SDL_RENDERER_TARGETTEXTURE);
            this->grid = new HexagonGrid(size, this->layout, this->renderer);
            FieldMeta *center = this->grid->get_field({0, 0, 0});
            this->field_box = new FieldBox(this->renderer, {0, 0, 200, 100}, fg, this->font, center);
            this->upgrade_box = new UpgradeBox(this->renderer, {0, 0, 200, 20}, fg, this->font, center);
            this->next_turn_button = new NextTurnButtonBox(this->renderer,
                                                           {window_size.x - 100, window_size.y - 100, 1, 1}, fg,
                                                           this->font, &(this->players));
            int font_height = TTF_FontHeight(this->font);
            this->text_input_box = new TextInputBox(this->renderer, {0, 0, window_size.x, font_height}, fg, this->font);
            this->text_input_box->stop();
        }
        catch (const SDL_Exception &sdl_except)
        {
            std::cerr << sdl_except.what() << " happened when constructing game" << std::endl;
        }
        this->frame_timer = new Timer();
        this->move_timer = new Timer();
        this->turn = 0;
        Player::current_player = this->players[turn];
    }

    ~Game()
    {
        for (auto player : this->players)
        {
            delete player;
        }
        delete text_input_box;
        delete this->next_turn_button;
        delete this->upgrade_box;
        delete this->field_box;
        delete this->move_timer;
        delete this->frame_timer;
        delete this->grid;
        delete this->renderer;
        delete this->window;
        delete this->layout;
        if (this->adding != nullptr)
        {
            delete this->adding;
        }
    }

    void start();

    void command(std::string command);

    void render();

    void handle_event(SDL_Event *event);

    int game_loop();

    void next_turn();

private:
    bool started;
    Player *adding;
    Uint64 turn;
    TextInputBox *text_input_box;
    std::vector<Player *> players;
    NextTurnButtonBox *next_turn_button;
    UpgradeBox *upgrade_box;
    FieldBox *field_box;
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

#include "Bob.hpp"


void Game::handle_event(SDL_Event *event)
{
    static SDL_Point window_size;
    std::string input;
    std::ostringstream prompt;
    switch (event->type)
    {
        case (SDL_QUIT):
            quit = true;
            break; // ignore input
        case SDL_WINDOWEVENT:
            if (event->window.windowID == this->window->get_window_id())
            {
                switch (event->window.event)
                {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        this->grid->update_dimensions({event->window.data1, event->window.data2});
                        this->text_input_box->update_dimensions({0, 0, event->window.data1,
                                                                 event->window.data2});
                        break;
                    default:
                        break;
                }
            }
            break;
        case SDL_MOUSEMOTION:
            if (!this->text_input_box->get_active())
            {
                grid->handle_event(event);
                this->field_box->handle_event(event);
                this->upgrade_box->handle_event(event);
            }
            break;
        case SDL_MOUSEWHEEL:
            if (!this->text_input_box->get_active())
            {
                grid->handle_event(event);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!this->text_input_box->get_active())
            {
                this->grid->handle_event(event);
                this->field_box->handle_event(event);
                if (this->started)
                {
                    this->upgrade_box->handle_event(event);

                }
            }
            break;
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym)
            {
                case SDLK_w:
                    if (!this->text_input_box->get_active())
                        this->move[0] = true;
                    break;
                case SDLK_a:
                    if (!this->text_input_box->get_active())
                        this->move[1] = true;
                    break;
                case SDLK_s:
                    if (!this->text_input_box->get_active())
                        this->move[2] = true;
                    break;
                case SDLK_d:
                    if (!this->text_input_box->get_active())
                        this->move[3] = true;
                    break;
                case SDLK_f:
                    if (!this->text_input_box->get_active())
                    {
                        window_size = this->window->toggle_fullscreen();
                        this->grid->update_dimensions(window_size);
                        this->text_input_box->update_dimensions({0, 0, window_size.x, 0});
                        this->upgrade_box->set_visible(false);
                    }
                    break;
                case SDLK_ESCAPE:
                    if (this->text_input_box->get_active())
                    {
                        this->text_input_box->stop();
                    }
                    else
                    {
                        this->text_input_box->start();
                    }
                    break;
                case SDLK_RETURN:
                    input = this->text_input_box->get_input();
                    if (this->text_input_box->get_active())
                    {
                        this->command(input);
                    }
                    break;
                default:
                    break;
            }
            if (this->text_input_box->get_active())
            {
                this->text_input_box->handle_event(event);
            }
            break;
        case SDL_KEYUP:
            switch (event->key.keysym.sym)
            {
                case SDLK_w:
                    this->move[0] = false;
                    break;
                case SDLK_a:
                    this->move[1] = false;
                    break;
                case SDLK_s:
                    this->move[2] = false;
                    break;
                case SDLK_d:
                    this->move[3] = false;
                    break;
                default:
                    break;
            }
            break;
        case SDL_TEXTINPUT:
            if (this->text_input_box->get_active())
            {
                this->text_input_box->handle_event(event);
            }
            break;
        case SDL_TEXTEDITING:
            if (this->text_input_box->get_active())
            {
                this->text_input_box->handle_event(event);
            }
            break;
        default:
            if ((event->type == BOB_MARKERUPDATE || event->type == BOB_FIELDSELECTEDEVENT)
                && !this->text_input_box->get_active())
            {
                this->grid->handle_event(event);
                this->field_box->handle_event(event);
                if (this->started)
                {
                    this->upgrade_box->handle_event(event);
                }
                for (Player *p : this->players)
                {
                    p->handle_event(event);
                }
            }
            if (event->type == BOB_NEXTROUNDEVENT || event->type == BOB_FIELDUPDATEEVENT
                || event->type == BOB_FIELDUPGRADEVENT || event->type == BOB_NEXTTURNEVENT)
            {
                this->grid->handle_event(event);
                this->field_box->handle_event(event);
                this->upgrade_box->handle_event(event);
                for (Player *p : this->players)
                {
                    p->handle_event(event);
                }
            }
            else if (event->type == BOB_FIELDSELECTEDEVENT && !this->started)
            {
                FieldMeta *field = (FieldMeta *) event->user.data1;
                if (event->user.code == 0 && this->adding != nullptr)
                {
                    if (this->grid->place(this->adding, field))
                    {
                        this->players.push_back(this->adding);
                        prompt << "Added Player: " << this->adding->get_name();
                        this->adding = nullptr;
                    }
                    else
                    {
                        prompt << "Failed to add Player: " << this->adding->get_name();
                        delete this->adding;
                    }
                }
                this->text_input_box->prompt(prompt.str());
            }
            break;
    }
}

void Game::command(std::string input)
{
    std::ostringstream prompt;
    if (input == "quit")
    {
        prompt << "Quitting the game";
        this->quit = true;
    }
    else if (input == "test")
    {
        prompt << "This is a test!";
    }
    else if (input == "debug")
    {
        //this->debug->toggle();
    }
    else if (input == "next")
    {
        if (this->started)
        {
            Player *last_player = Player::current_player;
            this->turn = (this->turn + 1) % players.size();
            Player::current_player = players[turn];
            if (this->turn == 0)
            {
                trigger_event(BOB_NEXTTURNEVENT, 0, (void *) last_player, (void *) Player::current_player);
                trigger_event(BOB_NEXTROUNDEVENT, 0, (void *) last_player, (void *) Player::current_player);
            }
            else
            {
                trigger_event(BOB_NEXTTURNEVENT, 0, (void *) last_player, (void *) Player::current_player);
            }
            prompt << "Next player is: " << (Player::current_player)->get_name();
        }
        else
        {
            prompt << "The game was not started yet!";
        }
    }
    else if (input == "surrender")
    {
        //Player::current_player->surrender();
    }
    else if (input.substr(0, 10) == "add player")
    {
        if (!this->started)
        {
            if (this->adding != nullptr)
            {
                this->grid->set_selecting(false);
                prompt << "Failed to add player " << this->adding->get_name() << "!df\n";
                delete this->adding;
            }
            this->grid->set_selecting(true);
            this->adding = new Player(input.substr(11, std::string::npos));
            prompt << "Select a place for " << this->adding->get_name() << ", please!";
            this->text_input_box->stop();
        }
        else
        {
            prompt << "The game has already been started. No additional player will be accepted for this game. ";
        }
    }
    else if (input == "start")
    {
        if (this->players.size() < 2)
        {
            prompt << "Please add at least one player, before starting the game.";
        }
        else if (!this->started)
        {
            this->start();
            this->started = true;
            prompt << "Started the game.";
        }
        else
        {
            prompt << "The game has already been started!";
        }
    }
    this->text_input_box->prompt(prompt.str());
}

void Game::start()
{
    this->players.erase(players.begin()); // remove default player from vector
    std::random_shuffle(players.begin(), players.end());
    this->turn = 0;
    Player::current_player = players[0];
    trigger_event(BOB_NEXTROUNDEVENT, 0, nullptr, (void *) Player::current_player);
    trigger_event(BOB_NEXTTURNEVENT, 0, nullptr, (void *) Player::current_player);
}

int Game::game_loop()
{
    this->frame_timer->start_timer();
    this->move_timer->start_timer();
    double fps;
    Uint32 frame_counter = 0;
    while (!this->quit)
    {
        if (this->move_timer->get_timer() > 1)
        {
            this->move_timer->reset_timer();
            SDL_Point move_by = {(this->move[1] - this->move[3]) * 20, (this->move[0] - this->move[2]) * 20};
            if (move[0] || move[1] || move[2] || move[3])
            {
                this->grid->move(move_by);
            }
        }
        if (this->frame_timer->get_timer() > 255)
        {
            fps = frame_counter / (this->frame_timer->reset_timer() / 1000.0);
            std::cout << fps << std::endl;
            frame_counter = 0;
        }
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            this->handle_event(&event);
        }
        this->render();
        frame_counter++;
    }
    this->renderer->clear();
    return 0;
}

void Game::render()
{
    try
    {
        this->renderer->set_draw_color({0x0, 0x0, 0x0, 0xff});
        this->renderer->clear();
        this->grid->render(this->renderer);
        this->field_box->render(this->renderer);
        this->upgrade_box->render(this->renderer);
        this->text_input_box->render(this->renderer);
        this->renderer->present();
    }
    catch (const SDL_RendererException &err)
    {
        std::cerr << "Failed to render: " << err.what() << std::endl;
    }
}

void init_sdl(Uint32 flags)
{
    if (SDL_Init(flags) != 0)
    {
        throw SDL_Exception("Failed to initialize SDL!");
    }
}

void init_ttf()
{
    if (TTF_Init() == -1)
    {
        throw SDL_TTFException();
    }
}

int main(int, char **)
{
    try
    {
        init_sdl(SDL_INIT_VIDEO);
        init_ttf();
    }
    catch (const SDL_Exception &sdl_err)
    {
        std::cerr << sdl_err.what() << std::endl;
        SDL_Quit();
    }
    SDL_Rect bounds;
    SDL_GetDisplayBounds(0, &bounds);
    SDL_Rect window_dimensions = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600};
    Game *game = new Game(&window_dimensions, 10);
    int exit_status = 1;
    exit_status = game->game_loop();
    delete game;
    TTF_Quit();
    SDL_Quit();
    return exit_status;
}
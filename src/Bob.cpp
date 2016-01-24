#include "Bob.hpp"

void Game::handle_event(SDL_Event *event)
{
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
                        this->grid->update_box({event->window.data1, event->window.data2});
                        break;
                    default:
                        break;
                }
            }
            break;
        case SDL_MOUSEMOTION:
            grid->handle_event(event, this->event_context);
            break;
        case SDL_MOUSEWHEEL:
            grid->handle_event(event, this->event_context);
            break;
        case SDL_MOUSEBUTTONDOWN:
            grid->handle_event(event, this->event_context);
            break;
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym)
            {
                case SDLK_w:
                    this->move[0] = true;
                    break;
                case SDLK_a:
                    this->move[1] = true;
                    break;
                case SDLK_s:
                    this->move[2] = true;
                    break;
                case SDLK_d:
                    this->move[3] = true;
                    break;
                case SDLK_f:
                    this->grid->update_box(this->window->toggle_fullscreen());
                    break;
                case SDLK_ESCAPE:
                    this->quit = true;
                    break;
                default:
                    break;
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
        case SDL_USEREVENT:
            switch (event->type - this->event_context->base_event)
            {
                case BOB_NEXTTURNEVENT:
                    this->grid->handle_event(event, this->event_context);
                default:
                    break;
            }
        default:
            break;
    }
}

int Game::game_loop()
{
    this->frame_timer->start_timer();
    this->move_timer->start_timer();
    double fps;
    Uint32 frame_counter = 0;
    while (!this->quit)
    {
        if (this->move_timer->get_timer() > 16)
        {
            this->move_timer->reset_timer();
            SDL_Point move_by = {(this->move[1] - this->move[3]) * 20, (this->move[0] - this->move[2]) * 20};
            this->grid->move(move_by);
        }
        if (this->frame_timer->get_timer() > 255)
        {
            fps = frame_counter / (this->frame_timer->reset_timer() / 1000.0);
            frame_counter = 0;
            this->test_box->load_text(std::to_string(fps));
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
        this->renderer->set_draw_color({0x0, 0x0, 0x0, 0x0});
        this->renderer->clear();
        this->grid->render(this->renderer->get_renderer());
        this->side_bar->render(this->renderer);
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
    SDL_Rect window_dimensions = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGTH};
    Game *game = new Game(&window_dimensions, 6);
    int exit_status = game->game_loop();
    delete game;
    TTF_Quit();
    SDL_Quit();
    return exit_status;
}
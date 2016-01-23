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
            grid->handle_event(event);
            break;
        case SDL_MOUSEWHEEL:
            grid->handle_event(event);
            break;
        case SDL_MOUSEBUTTONDOWN:
            grid->handle_event(event);
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
        if (move_timer->get_timer() > 16)
        {
            move_timer->reset_timer();
            SDL_Point move_by = {(this->move[1] - this->move[3]) * 20, (this->move[0] - this->move[2]) * 20};
            this->grid->move(move_by);
        }
        if (frame_timer->get_timer() > 1000.0)
        {
            fps = frame_counter / (frame_timer->reset_timer() / 1000.0);
            frame_counter = 0;
            std::cout << fps << " fps" << std::endl;
        }
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            this->handle_event(&event);
        }
        this->renderer->set_draw_color({0x0, 0x0, 0x0, 0xf});
        this->renderer->clear();
        this->renderer->set_draw_color({0xff, 0xff, 0xff, 0xff});
        this->render();
        this->renderer->present();
        frame_counter++;
    }
    this->renderer->clear();
    return 0;
}

void Game::render()
{
    SDL_Renderer *renderer = this->renderer->get_renderer();
    this->grid->render(renderer);
}

int main(int, char **)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        logSDLError(std::cerr, "SDL_init");
    }
//    register_events(4);
    SDL_Rect window_dimensions = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGTH};
    Game *game = new Game(&window_dimensions, 6);
    int exit_status = game->game_loop();
    delete game;
    SDL_Quit();
    return exit_status;
}
#include <string>
#include <SDL_video.h>
#include "Main.hpp"


void logSDLError(std::ostream &os, const std::string &msg)
{
    os << msg << " error:" << SDL_GetError() << std::endl;
}

SDL_Window *init_window()
{

    SDL_Window *window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGTH, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        logSDLError(std::cout, "SDL_CreateWindow");
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    return window;
}

SDL_Renderer *init_renderer(SDL_Window *window)
{
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        logSDLError(std::cout, "SDL_CreateRenderer");

    }
    return renderer;
}

void Timer::start_timer()
{
    this->timer_started = SDL_GetTicks();
}

Uint32 Timer::get_timer()
{
    Uint32 ticks_passed = SDL_GetTicks() - this->timer_started;
    return ticks_passed;
}

Uint32 Timer::reset_timer()
{
    Uint32 ticks_passed = this->get_timer();
    this->timer_started = SDL_GetTicks();
    return ticks_passed;
}

void Game::toggle_fullscreen()
{
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(this->window), &dm);
    if (!this->full_screen)
    {
        this->full_screen = true;
        SDL_SetWindowSize(this->window, dm.w, dm.h);
        SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN);
    }
    else
    {
        this->full_screen = false;
        SDL_SetWindowFullscreen(this->window, 0);
        SDL_SetWindowSize(this->window, SCREEN_WIDTH, SCREEN_HEIGTH);
        SDL_SetWindowPosition(this->window, dm.w / 4, dm.h / 4);
    }
}

void Game::handle_event(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN)
    {
        SDL_Keycode key = event->key.keysym.sym;
        if (key == SDLK_w)
            this->move[0] = true;
        if (key == SDLK_a)
            this->move[1] = true;
        if (key == SDLK_s)
            this->move[2] = true;
        if (key == SDLK_d)
            this->move[3] = true;
        if (key == SDLK_f)
        {
            toggle_fullscreen();
        }
        if (key == SDLK_ESCAPE)
        {
            this->quit = true;
        }
    }
    if (event->type == SDL_KEYUP)
    {
        SDL_Keycode key = event->key.keysym.sym;
        if (key == SDLK_w)
            this->move[0] = false;
        if (key == SDLK_a)
            this->move[1] = false;
        if (key == SDLK_s)
            this->move[2] = false;
        if (key == SDLK_d)
            this->move[3] = false;
    }
}

int Game::game_loop()
{
    Timer *frame_timer = new Timer();
    frame_timer->start_timer();
    double fps;
    Uint32 frame_counter = 0;
    while (!this->quit)
    {
        if (frame_timer->get_timer() > 1000.0)
        {
            fps = frame_counter / (frame_timer->reset_timer() / 1000.0);
            frame_counter = 0;
            std::cout << fps << std::endl;
        }
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            {
                this->handle_event(&event);
            }
            if (event.type == SDL_QUIT)
            {
                quit = true;
                break;
            }
            if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEWHEEL)
            {
                grid->handle_event(&event);
            }
        }
        SDL_Point move_by = {(this->move[1] - this->move[3]) * 10, (this->move[0] - this->move[2]) * 10};
        this->grid->move(move_by);
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        this->grid->render(renderer);
        SDL_RenderPresent(renderer);
        frame_counter++;
    }
    delete frame_timer;
    return 0;
}

int main(int, char **)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        logSDLError(std::cout, "SDL_init");
        return -1;
    }
    SDL_Window *window = init_window();
    if (!window)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    SDL_Renderer *renderer = init_renderer(window);
    if (!renderer)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    Game *game = new Game(window, renderer);
    int exit_status = game->game_loop();
    delete game;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return exit_status;
}


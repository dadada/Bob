//
// Created by tim on 26.01.16.
//

#ifndef BOB_WRAPPER_H
#define BOB_WRAPPER_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "Exceptions.hpp"
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <sstream>

SDL_Color operator!(const SDL_Color &color);

class Window
{
private:
    SDL_Window *window;
    const SDL_Rect *initial_dimensions;
    bool fullscreen;
public:
    Window(std::string title, SDL_Rect *dimensions, Uint32 flags)
    {
        this->window = SDL_CreateWindow(title.c_str(), dimensions->x, dimensions->y, dimensions->w, dimensions->h,
                                        flags);
        if (this->window == nullptr)
        {
            SDL_DestroyWindow(this->window);
            throw SDL_WindowException();
        }
        this->initial_dimensions = dimensions;
    }

    ~Window()
    {
        SDL_DestroyWindow(this->window);
    }

    SDL_Window *get_window() { return this->window; }

    SDL_Point get_size();

    SDL_Point toggle_fullscreen();

    int get_window_id();
};

class Renderer
{
public:
    Renderer(Window *window, int index, Uint32 flags)
    {
        this->renderer = SDL_CreateRenderer(window->get_window(), index, flags);
        if (renderer == nullptr)
        {
            SDL_DestroyRenderer(this->renderer);
            throw SDL_RendererException();
        }
    }

    ~Renderer()
    {
        SDL_DestroyRenderer(this->renderer);
    }

    SDL_Renderer *get_renderer() { return this->renderer; }

    void set_draw_color(SDL_Color color);

    void clear();

    void present();

    void set_target(SDL_Texture *texture);

    void copy(SDL_Texture *texture, SDL_Rect *src, SDL_Rect *dst);

    void set_blend_mode(SDL_BlendMode mode);

    void fill_rect(SDL_Rect *rect);

private:
    SDL_Renderer *renderer;
};

#endif //BOB_WRAPPER_H

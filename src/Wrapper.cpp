//
// Created by tim on 26.01.16.
//
#include "Wrapper.hpp"

void Renderer::set_target(SDL_Texture *texture)
{
    if (SDL_SetRenderTarget(this->renderer, texture) < 0)
    {
        throw SDL_RendererException();
    }
}

void Renderer::copy(SDL_Texture *texture, SDL_Rect *src, SDL_Rect *dst)
{
    if (SDL_RenderCopy(this->renderer, texture, src, dst) < 0)
    {
        throw SDL_RendererException();
    }
}

void Renderer::set_blend_mode(SDL_BlendMode mode)
{
    if (SDL_SetRenderDrawBlendMode(this->renderer, mode) < 0)
    {
        throw SDL_RendererException();
    }
}

void Renderer::fill_rect(SDL_Rect *rect)
{
    if (SDL_RenderFillRect(this->renderer, rect) < 0)
    {
        throw SDL_Exception("Failed to draw rectangle background!");
    }
}

SDL_Point Window::get_size()
{
    SDL_Point size;
    SDL_GetWindowSize(this->window, &size.x, &size.y);
    return size;
}
int Window::get_window_id()
{
    return SDL_GetWindowID(this->window);
}

void Renderer::set_draw_color(SDL_Color color)
{
    SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
}

void Renderer::clear()
{
    SDL_RenderClear(this->renderer);
}

void Renderer::present()
{
    SDL_RenderPresent(this->renderer);
}

SDL_Point Window::toggle_fullscreen()
{
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(this->window), &dm);
    if (!this->fullscreen)
    {
        this->fullscreen = true;
        SDL_SetWindowSize(this->window, dm.w, dm.h);
        SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN);
    }
    else
    {
        this->fullscreen = false;
        SDL_SetWindowFullscreen(this->window, 0);
        SDL_SetWindowSize(this->window, this->initial_dimensions->w, this->initial_dimensions->h);
        SDL_SetWindowPosition(this->window, this->initial_dimensions->x, this->initial_dimensions->y);
    }
    SDL_Point window_size = {0, 0};
    SDL_GetWindowSize(window, &(window_size.x), &(window_size.y));
    return window_size;
}

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "Exceptions.hpp"
#include "Gameplay.hpp"
#include "Events.hpp"

#ifndef _GUI_H
#define _GUI_H

void logSDLError(std::ostream &os, const std::string &msg);

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

    SDL_Point toggle_fullscreen();

    bool position_inside_window(SDL_Point position);

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

private:
    SDL_Renderer *renderer;
};

class InfoBox
{
public:
    InfoBox(Renderer *renderer_, SDL_Rect dimensions_, SDL_Color color_)
            : renderer(renderer_), color(color_), dimensions(dimensions_)
    {
        this->texture = SDL_CreateTexture(renderer->get_renderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                          dimensions.w, dimensions.h);
        if (this->texture == nullptr)
        {
            SDL_DestroyTexture(texture);
            throw SDL_TextureException();
        }
        this->visible = false;
    }

    ~InfoBox()
    {
        SDL_DestroyTexture(texture);
    }

    virtual void handle_event(const SDL_Event *event) const = 0;

    bool render(SDL_Renderer *renderer, const SDL_Rect target);

    void set_visible(bool visibility) { this->visible = visibility; }

    bool get_visible() { return this->visible; }

    SDL_Renderer *get_renderer() { return this->renderer->get_renderer(); }

    SDL_Rect get_dimensions() { return this->dimensions; }

protected:
    SDL_Rect dimensions;
    SDL_Texture *texture; // buffered texture
    Renderer *renderer;
    SDL_Color color;
    bool visible;

    bool create_texture_from_surface(SDL_Surface *surface);
};

class TextInfoBox : public InfoBox
{
public:
    TextInfoBox(Renderer *renderer, SDL_Rect dimensions, SDL_Color color, TTF_Font *font_)
            : InfoBox(renderer, dimensions, color), font(font_) { }

    virtual void handle_event(const SDL_Event *event);

    bool load_text(std::string text);

protected:
    TTF_Font *font;
};

class FieldInfoBox : public TextInfoBox
{
public:
    FieldInfoBox(Renderer *renderer, SDL_Rect dimensions, FieldMeta *field_, SDL_Color color, TTF_Font *font)
            : TextInfoBox(renderer, dimensions, color, font), field(field_) { }

    void handle_event(const SDL_Event *event);

private:
    FieldMeta *field;
};

class UpgradeInfoBox : public TextInfoBox
{
public:
    UpgradeInfoBox(Renderer *renderer, SDL_Rect dimensions, FieldMeta *field_, SDL_Color color, TTF_Font *font)
            : TextInfoBox(renderer, dimensions, color, font) { }

    void handle_event(const SDL_Event *event);
};

class ButtonInfoBox : public TextInfoBox
{
public:
    ButtonInfoBox(Renderer *renderer, SDL_Rect dimensions, FieldMeta *field_, SDL_Color color, TTF_Font *font,
                  UpgradeInfoBox *upgrade_box_)
            : TextInfoBox(renderer, dimensions, color, font), upgrade_box(upgrade_box_) { }

    void handle_event(const SDL_Event *event);

private:
    UpgradeInfoBox *upgrade_box;
};

class SideBar
{
public:
    SideBar(Renderer *renderer, SDL_Rect dimensions_, SDL_Color color)
            : dimensions(dimensions_)
    {
    }

    void handle_event(const SDL_Event *event);

    void render(SDL_Renderer *renderer);

private:
    SDL_Rect dimensions;
    FieldMeta *field;
    FieldInfoBox *field_info;
    std::vector<ButtonInfoBox *> *upgrades_list;
    UpgradeInfoBox *upgrade_info;
};

TTF_Font *load_font_from_file(std::string path_to_file);

#endif
#ifndef _GUI_H
#define _GUI_H

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

class Box
{
public:
    Box(Renderer *renderer_, SDL_Rect dimensions_, SDL_Color color_)
            : renderer(renderer_), color(color_), dimensions(dimensions_)
    {
        this->texture = nullptr;
        this->visible = false;
    }

    ~Box()
    {
        SDL_DestroyTexture(this->texture);
    }

    void update_dimensions(SDL_Point dimensions);

    virtual void render(Renderer *renderer);

    void set_visible(bool visibility) { this->visible = visibility; }

    SDL_Rect get_dimensions() { return this->dimensions; }

    virtual void handle_event(const SDL_Event *event) = 0;
protected:
    Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect dimensions;
    SDL_Color color;
    bool visible;
};

class TextBox : public Box
{
public:
    TextBox(Renderer *renderer, SDL_Rect dimensions, SDL_Color color, TTF_Font *font_)
            : Box(renderer, dimensions, color), font(font_) { }

    virtual bool load_text(std::string text);

    virtual void handle_event(const SDL_Event *event) { };

protected:
    TTF_Font *font;
};

class FieldBox : public TextBox
{
public:
    FieldBox(Renderer *renderer, SDL_Rect dimensions, SDL_Color color, TTF_Font *font, FieldMeta *field_)
            : TextBox(renderer, dimensions, color, font), field(field_) { }

    void handle_event(const SDL_Event *event);

    void update();

private:
    FieldMeta *field;
};

class UpgradeBox : public TextBox
{
public:
    UpgradeBox(Renderer *renderer, SDL_Rect dimensions, FieldMeta *field_, SDL_Color color, TTF_Font *font)
            : TextBox(renderer, dimensions, color, font) { }

    void handle_event(const SDL_Event *event);
};

class ButtonInfoBox : public TextBox
{
public:
    ButtonInfoBox(Renderer *renderer, SDL_Rect dimensions, FieldMeta *field_, SDL_Color color, TTF_Font *font,
                  UpgradeBox *upgrade_box_)
            : TextBox(renderer, dimensions, color, font), upgrade_box(upgrade_box_) { }

    void handle_event(const SDL_Event *event);

private:
    UpgradeBox *upgrade_box;
};

class Container
{
public:
    Container(Window *window_, Renderer *renderer_, SDL_Rect dimensions_)
            : window(window_), renderer(renderer_)
    {
        this->elements = std::vector<Box *>();
    }

    void add(Box *box) { this->elements.push_back(box); }

    void render(Renderer *renderer);

    void set_visible(bool visible);

    void handle_event(SDL_Event *event);

    void update_dimensions(SDL_Point dimensions);

private:
    Window *window;
    Renderer *renderer;
    std::vector<Box *> elements;
};

TTF_Font *load_font_from_file(std::string path_to_file, int size);

#endif
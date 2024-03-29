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
#include "Wrapper.hpp"
#include "Pixelmask.h"

class Box
{
public:
    Box(Renderer *renderer_, const SDL_Rect dimensions_, const SDL_Color color_)
            : renderer(renderer_), color(color_), dimensions(dimensions_)
    {
        this->texture = nullptr;
        this->visible = false;
    }

    virtual ~Box()
    {
        SDL_DestroyTexture(this->texture);
    }

    virtual void update_position(SDL_Point dimensions);

    virtual void render(Renderer *renderer);

    virtual void load() { this->changed = false; }

    virtual void set_visible(bool visibility) { this->visible = visibility; }

    SDL_Rect get_dimensions() { return this->dimensions; }

    virtual void handle_event(const SDL_Event *event) = 0;
protected:
    Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect dimensions;
    SDL_Color color;
    bool visible;
    bool changed;
};

class TextBox : public Box
{
public:
    TextBox(Renderer *renderer, SDL_Rect dimensions, SDL_Color color, TTF_Font *font_)
            : Box(renderer, dimensions, color), font(font_), lines(1)
    {
        this->load();
    }

    void load();

    void load_text(std::string text)
    {
        this->text = text;
        changed = true;
    }

    virtual void handle_event(const SDL_Event *event) { }

protected:
    TTF_Font *font;
    Uint16 lines;
    std::string text;
};

class TextInputBox : TextBox
{
public:
    TextInputBox(Renderer *renderer_, SDL_Rect dimensions_, SDL_Color color_, TTF_Font *font_)
            : TextBox(renderer_, dimensions_, color_, font_), input("")
    {
        this->visible = false;
        this->output << PlayerManager::pm->get_current().get_name() << "# ";
        this->load();
        this->load_text(output.str());
    }

    void start();

    void stop();

    bool get_active();

    void handle_event(const SDL_Event *event);

    void render(Renderer *ext_renderer);

    void update_dimensions(SDL_Rect rect);

    std::string get_input() { return this->input.str(); }

    void prompt(std::string message);

private:
    std::ostringstream output; // what is loaded to the texture - currnt input
    std::stringstream input; // editable command prompt
};

class FieldBox : public TextBox
{
public:
    FieldBox(Renderer *renderer, SDL_Rect dimensions, SDL_Color color, TTF_Font *font, FieldMeta *field_)
            : TextBox(renderer, dimensions, color, font), field(field_) { }

    void handle_event(const SDL_Event *event);

    virtual void update();

    void update_position(SDL_Point point);

protected:
    FieldMeta *field;
};

class UpgradeBox;

class UpgradeButtonBox : public TextBox
{
public:
    UpgradeButtonBox(Renderer *renderer, SDL_Rect dimensions, SDL_Color color, TTF_Font *font, UpgradeBox *box_,
                     Upgrade upgrade)
            : TextBox(renderer, dimensions, color, font), box(box_), upgrade(upgrade) { }

    Upgrade get_upgrade() { return this->upgrade; }

    void handle_event(const SDL_Event *event);

    void set_active(bool state);

private:
    UpgradeBox *box;
    Upgrade upgrade;
    bool active; // this upgrade has been unlocked
};

class UpgradeBox : public Box
{
public:
    UpgradeBox(Renderer *renderer, SDL_Rect dimensions, SDL_Color color, TTF_Font *font, FieldMeta *field_)
            : Box(renderer, dimensions, color), field(field_)
    {
        for (Upgrade upgrade : UPGRADES)
        {
            UpgradeButtonBox *box = new UpgradeButtonBox(renderer, {0, dimensions.y, dimensions.w, 20}, color, font,
                                                         this, upgrade);
            box->load_text(UPGRADE_NAMES.at(upgrade));
            this->marked_upgrade = box;
            this->upgrades.push_back(box);
        }
        this->upgrade_info = new TextBox(renderer, {0, 0, dimensions.w, 200}, color, font);
    }

    ~UpgradeBox()
    {
        for (auto box : upgrades)
        {
            delete box;
        }
        delete upgrade_info;
    }

    void handle_event(const SDL_Event *event);

    void render(Renderer *renderer);

    FieldMeta *get_field() { return this->field; }

    void update_position(SDL_Point pos);

    void set_visible(bool status);

    void update_upgrade_boxes();

private:
    std::vector<UpgradeButtonBox *> upgrades;
    UpgradeButtonBox *marked_upgrade;
    TextBox *upgrade_info;
    FieldMeta *field;
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

    void update_position(SDL_Point dimensions);

private:
    Window *window;
    Renderer *renderer;
    std::vector<Box *> elements;
};

TTF_Font *load_font_from_file(std::string path_to_file, int size);

#endif
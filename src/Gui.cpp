#include "Gui.hpp"

SDL_Color operator!(const SDL_Color &color)
{
    Uint8 r = (Uint8) 0xff - color.r;
    Uint8 g = (Uint8) 0xff - color.g;
    Uint8 b = (Uint8) 0xff - color.b;
    Uint8 a = (Uint8) 0xff - color.a;
    return {r, g, b, a};
}

TTF_Font *load_font_from_file(std::string path_to_file, int size)
{
    TTF_Font *font = TTF_OpenFontIndex(path_to_file.c_str(), size, 0);
    if (font == nullptr)
    {
        throw SDL_TTFException();
    }
    return font;
}

bool TextBox::load_text(std::string text)
{
    const char *displayed_text = text.c_str();
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(this->font, displayed_text, this->color, 100);
    if (surface == nullptr)
    {
        SDL_FreeSurface(surface);
        throw SDL_TTFException();
    }
    this->dimensions.w = surface->w;
    this->dimensions.h = surface->h;
    if (this->texture != nullptr)
    {
        SDL_DestroyTexture(this->texture);
    }
    SDL_SetRenderTarget(this->renderer->get_renderer(), this->texture);
    this->texture = SDL_CreateTextureFromSurface(this->renderer->get_renderer(), surface);
    SDL_FreeSurface(surface);
    if (this->texture == nullptr)
    {
        SDL_DestroyTexture(this->texture);
        throw SDL_TextureException();
    }
    SDL_SetRenderTarget(this->renderer->get_renderer(), nullptr); // reset the render target
    return (this->texture != nullptr);
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

bool inside_target(const SDL_Rect *target, const SDL_Point *position)
{
    return target->x < position->x && target->x + target->w > position->x && target->y < position->y &&
           target->y + target->h > position->y;
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

void Container::handle_event(SDL_Event *event)
{
    for (auto box : this->elements)
        box->handle_event(event);
}

void FieldBox::handle_event(const SDL_Event *event)
{
    if (event->type == BOB_FIELDUPDATEEVENT)
    {
        FieldMeta *field_update = reinterpret_cast<FieldMeta *>(event->user.data1);
        if (field_update == this->field)
        {
            this->update();
        }
    }
    else if (event->type == BOB_MARKERUPDATE)
    {
        if (event->user.code == 0)
        {
            if (!Timer::MOUSE_LOCKED)
            {
                SDL_Point mouse;
                SDL_GetMouseState(&mouse.x, &mouse.y);
                this->update_position(mouse);
                this->field = static_cast<FieldMeta *>(event->user.data1);
            }
            this->visible = true;
        }
        else if (!Timer::MOUSE_LOCKED)
        {
            this->visible = false;
        }
        this->update();
    }
}

void FieldBox::update()
{
    HexagonGrid *grid = this->field->get_grid();
    Cluster cluster = grid->get_cluster(this->field);
    Resource cluster_resources = grid->get_resources_of_cluster(&cluster);
    Resource field_resources = this->field->get_resources();
    std::ostringstream output;
    output << this->field->get_owner()->get_name() << "\n"
    << "● " << (int) cluster_resources.circle << " (" << (int) field_resources.circle << ")" << "\n"
    << "▲ " << (int) cluster_resources.triangle << " (" << (int) field_resources.triangle << ")" << "\n"
    << "■ " << (int) cluster_resources.square << " (" << (int) field_resources.square << ")" << "\n";
    this->load_text(output.str());
}

void UpgradeButtonBox::set_active(bool state)
{
    this->active = state;
    if (state)
    {
        this->color.b = 0xff;
        this->load_text(UPGRADE_NAMES.at(this->upgrade));
    }
}

void UpgradeBox::handle_event(const SDL_Event *event)
{
    if (event->type == BOB_FIELDUPDATEEVENT)
    {
        // mark updates active / inactive
        this->field = static_cast<FieldMeta *>(event->user.data1);
        UpgradeFlags updated_upgrades = this->field->get_upgrades();
        for (auto upgrade : this->upgrades)
        {
            Upgrade up = upgrade->get_upgrade();
            bool active = updated_upgrades[up];
            upgrade->set_active(active);
        }
    }
    else if (this->visible)
    {
        if (event->type == SDL_MOUSEBUTTONDOWN)
        {
            this->marked_upgrade->handle_event(event);
            SDL_Point mouse;
            SDL_GetMouseState(&mouse.x, &mouse.y);
            if (!inside_target(&(this->dimensions), &mouse))
            {
                Timer::MOUSE_LOCKED = false;
                this->set_visible(false);
            }
        }
        else if (event->type == SDL_MOUSEMOTION && this->visible)
        {
            SDL_Point pos;
            SDL_GetMouseState(&(pos.x), &(pos.y));
            // update the info text field for the selected update
            for (auto box : this->upgrades)
            {
                SDL_Rect target = box->get_dimensions();
                if (this->marked_upgrade != box && inside_target(&target, &pos))
                {
                    this->marked_upgrade = box;
                    Resource costs = UPGRADE_COSTS.at(box->get_upgrade());
                    std::ostringstream output;
                    output //<< UPGRADE_NAMES.at(box->get_upgrade()) << "\n"
                    << "● " << (int) costs.circle << "\n"
                    << "▲ " << (int) costs.triangle << "\n"
                    << "■  " << (int) costs.square << "\n"
                    << UPGRADE_TEXTS.at(box->get_upgrade());
                    SDL_Rect box_dim = box->get_dimensions();
                    this->upgrade_info->update_position({box_dim.x + box_dim.w + 6, box_dim.y});
                    this->upgrade_info->load_text(output.str());
                }
            }
        }
    }
    else // NOT visible
    {
        if (event->type == BOB_FIELDSELECTED)
        {
            FieldMeta *selected = static_cast<FieldMeta *>(event->user.data1);
            if (selected != nullptr && event->user.code == 0x0)
            {
                Timer::MOUSE_LOCKED = true;
                SDL_Point mouse;
                SDL_GetMouseState(&mouse.x, &mouse.y);
                this->update_position(mouse);
                this->field = selected;
                this->set_visible(true);
            }
        }
    }
}

void UpgradeButtonBox::handle_event(const SDL_Event *event)
{
    SDL_Point pos;
    SDL_GetMouseState(&(pos.x), &(pos.y));
    if (event->type == SDL_MOUSEBUTTONDOWN && this->visible)
    {
        if (inside_target(&(this->dimensions), &pos))
        {
            FieldMeta *field = this->box->get_field();
            field->upgrade(this->upgrade);
        }
    }
}

void UpgradeBox::render(Renderer *ext_renderer)
{
    this->upgrade_info->render(ext_renderer);
    for (auto box : this->upgrades)
    {
        box->render(ext_renderer);
    }
    SDL_Rect dim = this->upgrades[0]->get_dimensions();
    this->dimensions.w = dim.w;
    this->dimensions.h = (dim.h + 4) * (int) this->upgrades.size();
}

void Container::render(Renderer *renderer)
{
    for (auto info_box : this->elements)
    {
        info_box->render(renderer);
    }
}

void Box::render(Renderer *ext_renderer)
{
    if (this->visible && this->texture != nullptr)
    {
        SDL_Color bg = {0xff, 0xff, 0xff, 0xff};
        renderer->set_draw_color(bg);
        if (SDL_SetRenderDrawBlendMode(ext_renderer->get_renderer(), SDL_BLENDMODE_NONE) < 0
            || SDL_RenderFillRect(ext_renderer->get_renderer(), &(this->dimensions)) < 0)
        {
            throw SDL_Exception("Failed to draw rectangle background!");
        }
        renderer->set_draw_color(this->color);
        if (SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND) < 0
            || SDL_RenderCopy(ext_renderer->get_renderer(), this->texture, nullptr, &(this->dimensions)) < 0)
        {
            throw SDL_RendererException();
        }
    }
}

void Container::set_visible(bool visible)
{
    for (auto box : this->elements)
    {
        box->set_visible(visible);
    }
}

void Container::update_position(SDL_Point dimensions)
{
    for (auto box : this->elements)
        box->update_position(dimensions);
}

void Box::update_position(SDL_Point pos)
{
    this->dimensions.x = pos.x;
    this->dimensions.y = pos.y;
}

void UpgradeBox::update_position(SDL_Point pos)
{
    SDL_Point d_pos = pos;
    this->dimensions.x = pos.x;
    this->dimensions.y = pos.y;
    for (auto box : this->upgrades)
    {
        box->update_position(d_pos);
        d_pos.y += 20;
    }
    this->upgrade_info->update_position({pos.x + 110, pos.y});
}

void UpgradeBox::set_visible(bool status)
{
    this->visible = status;
    this->upgrade_info->set_visible(status);
    for (UpgradeButtonBox *box : this->upgrades)
    {
        box->set_visible(status);
    }
}

void FieldBox::update_position(SDL_Point point)
{
    this->dimensions.x = point.x;
    this->dimensions.y = point.y - dimensions.h - 6;
}
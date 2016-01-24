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
    SDL_Surface *surface = TTF_RenderUTF8_Solid(this->font, text.c_str(), {0xff, 0xff, 0xff, 0xff});
    if (surface == nullptr || TTF_SizeUTF8(this->font, text.c_str(), &(this->dimensions.w), &(this->dimensions.h)))
    {
        SDL_FreeSurface(surface);
        throw SDL_TTFException();
    }
    else if (surface->w > this->dimensions.w || surface->h > this->dimensions.h)
    {
        std::cerr << "Overfull TextBox!" << std::endl;
    }
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

bool Window::position_inside_window(SDL_Point position)
{
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(this->window), &dm);
    return position.x > 0 && position.x < dm.w && position.y > 0 && position.y < dm.h;
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

void TextBox::handle_event(const SDL_Event *event, EventContext *context)
{
}

void FieldBox::handle_event(const SDL_Event *event, EventContext *context)
{
    std::ostringstream output;
    Cluster *cluster = this->field->get_grid()->get_cluster(this->field);
    Resource cluster_resources = this->field->get_grid()->get_resources_of_cluster(cluster);
    Resource field_resources = this->field->get_resources();
    FieldUpdate *update = (FieldUpdate *) event->user.data1;
/*    switch (event->type)
    {
        case (BOB_FIELD_UPDATE_EVENT):
            output << this->field->get_owner()->get_name() << std::endl;
            output << "● " << cluster_resources.circle << " (" << field_resources.circle << ")" << std::endl;
            output << "▲ " << cluster_resources.triangle << " (" << field_resources.triangle << ")" << std::endl;
            output << "■ " << cluster_resources.square << " (" << field_resources.square << ")" << std::endl;
            this->field_info->load_text(output.str());
            break;
        case (BOB_UPDATE_MARKER_EVENT):
            this->field = update->field;
            break;
        default:
            break;
    }*/
}

void ButtonInfoBox::handle_event(const SDL_Event *event, EventContext *context)
{
};

void UpgradeBox::handle_event(const SDL_Event *event, EventContext *context)
{
}

void Box::handle_event(const SDL_Event *event, EventContext *context)
{
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
        if (SDL_RenderCopy(ext_renderer->get_renderer(), this->texture, nullptr, &(this->dimensions)) < 0)
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
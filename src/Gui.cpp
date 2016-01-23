#include "Gui.hpp"

TTF_Font *load_font_from_file(std::string path_to_file)
{
    TTF_Font *font = TTF_OpenFont(path_to_file.c_str(), 12); // what about memory leaks?
    if (font == nullptr)
    {
        std::cerr << "Failed to load TTF!" << TTF_GetError() << std::endl;
        return font;
    }
    return font;
}

bool TextInfoBox::load_text(std::string text)
{
    SDL_DestroyTexture(this->texture);
    SDL_Surface *surface = TTF_RenderUTF8_Solid(this->font, text.c_str(), color);
    if (surface == nullptr)
    {
        std::cerr << "Unable to render text to surface! " << TTF_GetError() << std::endl;
    }
    else if (surface->w > this->dimensions.w || surface->h > this->dimensions.h)
    {
        std::cerr << "Overfull TextBox!" << SDL_GetError() << std::endl;
    }
    else
    {
        this->create_texture_from_surface(surface);
    }
    SDL_FreeSurface(surface);
    return (this->texture != nullptr);
}


bool InfoBox::create_texture_from_surface(SDL_Surface *surface)
{
    SDL_SetRenderTarget(this->renderer->get_renderer(), this->texture);
    this->texture = SDL_CreateTextureFromSurface(this->renderer->get_renderer(), surface);
    if (this->texture == nullptr)
    {
        std::cerr << "Unable to render texture from surface!" << SDL_GetError() << std::endl;
    }
    SDL_SetRenderTarget(this->renderer->get_renderer(), nullptr); // reset the render target
    return (this->texture != nullptr);
}

bool InfoBox::render(SDL_Renderer *renderer, const SDL_Rect target)
{
    if (this->visible)
    {
        if (!SDL_RenderCopy(renderer, this->texture, &(this->dimensions), &target))
        {
            std::cerr << "Failed to render TextBox Texture!" << SDL_GetError() << std::endl;
            return false;
        }
    }
    return true;
}

void logSDLError(std::ostream &os, const std::string &msg)
{
    os << msg << " error:" << SDL_GetError() << std::endl;
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

void TextInfoBox::handle_event(const SDL_Event *event)
{
}

void SideBar::handle_event(const SDL_Event *event)
{
    std::ostringstream output;
    Cluster *cluster = this->field->get_grid()->get_cluster(this->field);
    Resource cluster_resources = this->field->get_grid()->get_resources_of_cluster(cluster);
    Resource field_resources = this->field->get_resources();
    MarkerUpdate *update = (MarkerUpdate *) event->user.data1;
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

void FieldInfoBox::handle_event(const SDL_Event *event)
{
}

void ButtonInfoBox::handle_event(const SDL_Event *event)
{
    switch (event->type)
    {
        case (SDL_MOUSEBUTTONDOWN):
            this->upgrade_box->set_visible(!(this->upgrade_box->get_visible()));
            break;
        default:
            break;
    }
};

void UpgradeInfoBox::handle_event(const SDL_Event *event)
{
    TextInfoBox::handle_event(event);
}

void SideBar::render(SDL_Renderer *renderer)
{
    SDL_Rect f_dimensions = this->field_info->get_dimensions();
    this->field_info->render(renderer, {this->dimensions.x, this->dimensions.y, f_dimensions.w, f_dimensions.h});
    for (auto &elem : *upgrades_list)
    {
        f_dimensions = elem->get_dimensions();
        elem->render(renderer, {this->dimensions.x, this->dimensions.y, f_dimensions.w, f_dimensions.h});
    }
    f_dimensions = this->upgrade_info->get_dimensions();
    this->upgrade_info->render(renderer, {this->dimensions.x, this->dimensions.y, f_dimensions.w, f_dimensions.h});
}
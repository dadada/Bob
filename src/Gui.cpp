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

void TextBox::load()
{
    this->renderer->set_draw_color({0, 0, 0, 0xff});
    if (this->text.size() < 1)
    {
        this->text += " ";
    }
    const char *displayed_text = this->text.c_str();
    SDL_Surface *text_surface = TTF_RenderUTF8_Blended_Wrapped(this->font, displayed_text, this->color,
                                                               this->dimensions.w);
    if (text_surface == nullptr)
    {
        SDL_FreeSurface(text_surface);
        throw SDL_TTFException();
    }
    this->dimensions.h = text_surface->h;
    SDL_Surface *surface = SDL_CreateRGBSurface(0, this->dimensions.w, text_surface->h, 32, rmask, gmask, bmask,
                                                amask);
    if (surface == nullptr || SDL_LockSurface(surface) < 0 ||
        SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 255, 255, 255)) < 0)
    {
        throw SDL_Exception("Failed to fill rect behind text!");
    }
    SDL_UnlockSurface(surface);
    if (SDL_BlitSurface(text_surface, nullptr, surface, nullptr) < 0)
    {
        throw SDL_Exception("Failed to create background text_surface!");
    }
    if (this->texture != nullptr)
    {
        SDL_DestroyTexture(this->texture);
    }
    SDL_SetRenderTarget(this->renderer->get_renderer(), this->texture);
    this->texture = SDL_CreateTextureFromSurface(this->renderer->get_renderer(), surface);
    SDL_FreeSurface(text_surface);
    SDL_FreeSurface(surface);
    if (this->texture == nullptr)
    {
        SDL_DestroyTexture(this->texture);
        throw SDL_TextureException();
    }
    SDL_SetRenderTarget(this->renderer->get_renderer(), nullptr); // reset the render target
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
            changed = true;
        }
    }
    else if (event->type == BOB_MARKERUPDATE)
    {
        FieldMeta *field_update = reinterpret_cast<FieldMeta *>(event->user.data1);
        this->field = field_update;
        this->update();
        SDL_Point mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);
        this->update_position(mouse);
        this->visible = true;
        changed = true;
    }
}

void FieldBox::update()
{
    HexagonGrid *grid = this->field->get_grid();
    Cluster cluster = grid->get_cluster(this->field);
    Resource cluster_resources = grid->get_resources_of_cluster(&cluster);
    Resource field_resources = this->field->get_resources();
    std::ostringstream output;
    output << this->field->get_owner().get_name() << "\n"
    << "off " << this->field->get_offense() << "\ndef " << this->field->get_defense() << "\n"
    << "● " << (int) cluster_resources.circle << " (" << (int) field_resources.circle << ")" << "\n"
    << "▲ " << (int) cluster_resources.triangle << " (" << (int) field_resources.triangle << ")" << "\n"
    << "■ " << (int) cluster_resources.square << " (" << (int) field_resources.square << ")" << "\n";
    this->load_text(output.str());
    changed = true;
}

void UpgradeButtonBox::set_active(bool state)
{
    this->active = state;
    if (state)
    {
        this->color.b = 0xff;
        this->load_text(UPGRADE_NAMES.at(this->upgrade));
    }
    else
    {
        this->color = {0, 0, 0, 0xff};
        this->load_text(UPGRADE_NAMES.at(this->upgrade));
    }
    changed = true;
}

void UpgradeBox::update_upgrade_boxes()
{
    UpgradeFlags active_upgrades = this->field->get_upgrades();
    for (int i = 0; i < NUM_UPGRADES; i++)
    {
        this->upgrades[i]->set_active(active_upgrades[i]);
    }
    changed = true;
}

void UpgradeBox::handle_event(const SDL_Event *event)
{
    if (this->visible)
    {
        if (event->type == BOB_FIELDUPDATEEVENT)
        {
            this->update_upgrade_boxes();
            changed = true;
        }
        else if (event->type == SDL_MOUSEBUTTONDOWN)
        {
            SDL_Point mouse;
            SDL_GetMouseState(&mouse.x, &mouse.y);
            if (!inside_target(&(this->dimensions), &mouse))
            {
                Timer::MOUSE_LOCKED = false;
                this->set_visible(false);
            }
            this->marked_upgrade->handle_event(event);
            changed = true;
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
            changed = true;
        }
    }
    else // NOT visible
    {
        if (event->type == BOB_FIELDSELECTEDEVENT)
        {
            FieldMeta *selected = static_cast<FieldMeta *>(event->user.data1);
            if (selected != nullptr && event->user.code == 0x0 && !Timer::MOUSE_LOCKED)
            {
                Timer::MOUSE_LOCKED = true;
                SDL_Point mouse;
                SDL_GetMouseState(&mouse.x, &mouse.y);
                this->update_position(mouse);
                this->field = selected;
                this->update_upgrade_boxes();
                this->set_visible(true);
                changed = true;
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
            if (PlayerManager::pm->get_current() == field->get_owner())
            {
                field->upgrade(this->upgrade);
                changed = true;
            }
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
    this->changed = false;
    this->dimensions = this->upgrades[0]->get_dimensions();
    this->dimensions.h *= this->upgrades.size();
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
    if (changed)
    {
        this->load();
    }
    if (this->visible && this->texture != nullptr)
    {
        ext_renderer->copy(this->texture, nullptr, &(this->dimensions));
    }
    this->changed = false;
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
        d_pos.y += box->get_dimensions().h;
    }
    this->upgrade_info->update_position({pos.x + this->upgrades[0]->get_dimensions().w, pos.y});
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
    this->dimensions.y = point.y - this->dimensions.h - 6;
}

void TextInputBox::start()
{
    this->visible = true;
    SDL_StartTextInput();
}

void TextInputBox::stop()
{
    this->visible = false;
    SDL_StopTextInput();
}

void TextInputBox::handle_event(const SDL_Event *event)
{
    bool changed = false;
    if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.sym == SDLK_RETURN)
        {
            this->input.str("");
            changed = true;
        }
        else if (event->key.keysym.sym == SDLK_BACKSPACE)
        {
            std::string foo = input.str();
            if (!foo.empty())
                foo.pop_back();
            input.str(foo);
            input.seekp(foo.length());
            changed = true;
        }
        else if (event->key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
        {
            SDL_SetClipboardText(input.str().c_str());
        }
        else if (event->key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
        {
            input << SDL_GetClipboardText();
            changed = true;
        }
    }
    else if (event->type == SDL_TEXTINPUT)
    {
        if (!((event->text.text[0] == 'c' || event->text.text[0] == 'C')
              && (event->text.text[0] == 'v' || event->text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
        {
            input << event->text.text;
            changed = true;
        }
    }
    else if (event->type == SDL_TEXTEDITING)
    {
        // nothin atm
    }
    if (changed)
    {
        std::string text;
        text = output.str();
        text += input.str();
        if (text.empty())
            text += " ";
        this->load_text(text);
        this->changed = true;
    }
}

void TextInputBox::prompt(std::string message)
{
    this->output << this->input.str() << "\n" << message << "\n" << PlayerManager::pm->get_current().get_name() << "# ";
    this->lines += 2;
    this->load_text(output.str());
    if (lines > 20)
    {
        this->lines = 2;
        output.str("");
        this->prompt(message);
    }
    this->input.str("");
    this->changed = true;
}

void TextInputBox::render(Renderer *ext_renderer)
{
    if (this->changed)
    {
        this->load();
    }
    if (this->texture != nullptr && this->visible)
    {
        ext_renderer->copy(this->texture, nullptr, &(this->dimensions));
    }
    this->changed = false;
}

void TextInputBox::update_dimensions(SDL_Rect rect)
{
    this->dimensions.x = rect.x;
    this->dimensions.y = rect.y;
    this->dimensions.w = rect.w;
    this->changed = true;
}

bool TextInputBox::get_active()
{
    return SDL_IsTextInputActive() == true;
}
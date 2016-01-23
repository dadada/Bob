#include "Gameplay.hpp"

void FieldMeta::render(SDL_Renderer *renderer, Layout *layout)
{
    Point precise_location = this->field.field_to_point(layout);
    SDL_Point location;
    location.x = (int) precise_location.x;
    location.y = (int) precise_location.y;
    std::vector<Point> polygon = this->field.field_to_polygon(layout);
    SDL_Color color = this->owner->get_color();
    Sint16 vx[6];
    Sint16 vy[6];
    for (int i = 0; i < 6; i++)
    {
        vx[i] = (Sint16) polygon[i].x;
        vy[i] = (Sint16) polygon[i].y;
    }
    if (this->owner == this->grid->get_default_player())
        color = {0x77, 0x77, 0x77, 0x77};
    filledPolygonRGBA(renderer, vx, vy, 6, color.r, color.g, color.b, 0x33);
    SDL_Color inverse;
    inverse.r = (Uint8) (0xff - color.r);
    inverse.g = (Uint8) (0xff - color.g);
    inverse.b = (Uint8) (0xff - color.b);
    inverse.a = 0xff;

    Uint16 resource_size = (Uint16) (layout->size / 3);
    if (this->resources_base.circle > 0)
    {
        circleRGBA(renderer, (Sint16) (location.x), (Sint16) (location.y), (Sint16) (resource_size), inverse.r,
                   inverse.g, inverse.b, inverse.a);
    }
    if (this->resources_base.triangle > 0)
    {
        static const SDL_Point trigon[] = {{-1, 1},
                                           {1,  1},
                                           {0,  -1}};
        for (int i = 0; i < 3; i++)
        {
            vx[i] = (Sint16) (location.x + (trigon[i].x) * resource_size);
            vy[i] = (Sint16) (location.y + (trigon[i].y) * resource_size);
        }
        trigonRGBA(renderer, vx[0], vy[0], vx[1], vy[1], vx[2], vy[2], inverse.r, inverse.g, inverse.b, inverse.a);
    }

    if (this->resources_base.square > 0)
    {

        static const SDL_Point square[] = {{-1, -1},
                                           {-1, 1},
                                           {1,  1},
                                           {1,  -1}};
        for (int i = 0; i < 4; i++)
        {
            vx[i] = (Sint16) (location.x + (square[i].x) * resource_size);
            vy[i] = (Sint16) (location.y + (square[i].y) * resource_size);
        }
        polygonRGBA(renderer, vx, vy, 4, inverse.r, inverse.g, inverse.b, inverse.a);
    }
}

void FieldMeta::regenerate_resources()
{
    this->resources = resources_base;
    if (this->upgrades[Regeneration_1])
        this->resources *= 2;
    if (this->upgrades[Regeneration_2])
        this->resources *= 2;
    if (this->upgrades[Regeneration_3])
        this->resources *= 2;
}

Resource Grid::get_resources_of_cluster(const Cluster *cluster)
{
    Resource res = {0, 0, 0};
    for (FieldMeta *elem : *cluster)
    {
        Resource r_plus = elem->get_resources();
        res += r_plus;
    }
    return res;
}

bool FieldMeta::upgrade(Upgrade upgrade)
{
    // check available resources for cluster and consume resources
    Cluster *cluster = this->grid->get_cluster(this);
    Resource cluster_resources = this->grid->get_resources_of_cluster(this->grid->get_cluster(this, cluster));
    Resource remaining_costs;
    try
    {
        Resource costs = UPGRADE_COSTS.at(upgrade);
        remaining_costs = this->grid->consume_resources_of_cluster(cluster, costs);
    }
    catch (const std::out_of_range &oor)
    {
        std::cerr << "Out of Range exception: " << oor.what() << std::endl;
        remaining_costs = {1, 1, 1};
    }
    static const Resource neutral = {0, 0, 0};
    if (remaining_costs == neutral)
    {
        this->upgrades[upgrade] = true;
    }
    return this->upgrades[upgrade];
}

FieldMeta *FieldMeta::get_neighbor(Uint8 direction)
{
    return this->grid->get_neighbor(this, direction);
}

FieldMeta *HexagonGrid::get_neighbor(FieldMeta *meta, Uint8 direction)
{
    Field neighbor_field = meta->get_field().get_neighbor(direction);
    try
    {
        return this->fields.at(neighbor_field);
    }
    catch (const std::out_of_range &oor)
    {
        std::cerr << "Tried to look up non-existing field: " << neighbor_field << std::endl;
    }
    return nullptr;
}

Cluster *HexagonGrid::get_cluster(FieldMeta *field, Cluster *visited)
{
    if (visited == nullptr)
        visited = new Cluster();
    if (visited->find(field) != visited->end()) // already been here before
        return visited;
    else
        visited->insert(field);
    for (Uint8 i = 0; i < 6; i++)
    {
        FieldMeta *neighbor = this->get_neighbor(field, i);
        if (neighbor->get_owner() != field->get_owner()) // ignore meta if field not owned by specified owner
            return visited;
        this->get_cluster(neighbor, visited);
    }
    return visited;
}

Resource Grid::consume_resources_of_cluster(Cluster *cluster, Resource costs)
{
    for (FieldMeta *meta : *cluster)
    {
        // mind the "special" definition of -=, only byte of what you can chew or leave nothing behind
        Resource tmp = costs;
        costs -= meta->get_resources();
        meta->consume_resources(tmp);
    }
    delete cluster;
    return costs; // > {0, 0, 0} means there were not enough resources
}

bool Player::fight(FieldMeta *field)
{
    if (*this == *(field->get_owner())) // attacked field outside of the map or friendly fire
    {
        return false;
    }
    if (field->get_owner() == field->get_grid()->get_default_player()) // still to be had
    {
        field->set_owner(this);
        return true;
    }

    // defending player's defense against attacking player's offense
    int power_level = field->get_defense(); // it's over 9000
    for (Uint8 i = 0; i < 6; i++)
    {
        FieldMeta *neighbor = field->get_neighbor(i);
        if (neighbor == nullptr) // there is no neighbor in this direction
        {
            continue;
        }
        if (*(neighbor->get_owner()) == *this) // comparison by UUID
            power_level -= neighbor->get_offense();
        else if (*(neighbor->get_owner()) == *(field->get_owner()))
            power_level += neighbor->get_defense();
        // else ignore, field / player not part of the fight (e.g. default player)
    }
    if (power_level < 0) // attacking player has won
    {
        field->set_owner(this);
        return true;
    }
    return false;
}

void FieldMeta::handle_event(const SDL_Event *event)
{
    this->regenerate_resources();
}

bool HexagonGrid::render(SDL_Renderer *renderer)
{
    Field some_field = {0, 0, 0};
    std::vector<Point> polygon = some_field.field_to_polygon_normalized(this->layout);
    assert(polygon.size() > 5);
    Sint16 x[6];
    Sint16 y[6];
    for (std::pair<Field, FieldMeta *> const &elem : this->fields)
    {
        Field field = elem.first;
        Point center = field.field_to_point(this->layout);
        for (uint8_t i = 0; i < 6; i++)
        {
            x[i] = (Sint16) (center.x + polygon[i].x);
            y[i] = (Sint16) (center.y + polygon[i].y);
        }
        const SDL_Color color = {0xff, 0xff, 0xff, 0xff};
        polygonRGBA(renderer, x, y, 6, color.r, color.g, color.b, color.a);
        if (elem.first == this->marker->get_field())
        {
            filledPolygonRGBA(renderer, x, y, 6, 0x77, 0x77, 0x77, 0x77);
        }
        elem.second->render(renderer, this->layout);
    }
    return true;
}

void Grid::handle_event(SDL_Event *event)
{
    if (event->type == SDL_MOUSEWHEEL)
    {
        SDL_Point mouse = {0, 0};
        SDL_GetMouseState(&mouse.x, &mouse.y);
        int scroll = this->layout->size / 10 * event->wheel.y;
        double old_size = this->layout->size;
        SDL_Point old_origin = this->layout->origin;
        if (old_size + scroll < 10)
        {
            this->layout->size = 10;
        }
        else if (old_size + scroll > 100)
        {
            this->layout->size = 100;
        }
        else
        {
            this->layout->size += scroll;
        }
        this->move(((1.0 - (double) this->layout->size / old_size) * (mouse - old_origin)));
    }
    if (event->type == SDL_MOUSEMOTION)
    {
        if (this->panning)
        {
            SDL_Point mouse = {0, 0};
            SDL_GetMouseState(&mouse.x, &mouse.y);
            Point marker_pos = this->marker->get_field().field_to_point(this->layout);
            SDL_Point p;
            p.x = (int) marker_pos.x;
            p.y = (int) marker_pos.y;
            this->move(mouse - p);
        }
        this->update_marker();
    }
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_MIDDLE)
    {
        this->panning = !(this->panning);
    }
}

void Grid::move(SDL_Point m)
{
    this->layout->origin = this->layout->origin + m;
    // check if some part is inside layout->box
    if (!on_rectangle(&layout->box))
        this->layout->origin = this->layout->origin - m;
    this->update_marker();
}

void Grid::update_marker()
{
    SDL_Point m = {0, 0};
    SDL_GetMouseState(&(m.x), &(m.y));
    Point p = {0.0, 0.0};
    p.x = m.x;
    p.y = m.y;
    FieldMeta *n_marker = this->point_to_field(p);
    if (n_marker != nullptr)
        this->marker = n_marker;
}

FieldMeta *Grid::point_to_field(const Point p)
{
    Field field = p.point_to_field(this->layout);
    FieldMeta *meta = nullptr;
    try
    {
        meta = this->fields.at(field);
    }
    catch (const std::out_of_range &oor)
    {
        //std::cerr << "Tried to access non-existant field " << field <<  ": " << oor.what() << std::endl;
    }
    return meta;

}

bool Grid::on_rectangle(SDL_Rect *rect)
{
    // check if center inside rect for ANY field
    for (const auto &pair : this->fields)
    {
        Point precise_p = pair.first.field_to_point(layout);
        SDL_Point p;
        p.x = (int) precise_p.x;
        p.y = (int) precise_p.y;
        if (p.x > rect->x && p.y > rect->y && p.x < (rect->x + rect->w) && p.y < (rect->y + rect->h))
        {
            return true;
        }
    }
    return false;
}

void Grid::update_box(SDL_Point dimensions)
{
    this->layout->box.w = dimensions.x;
    this->layout->box.h = dimensions.y;
}

std::ostream &operator<<(std::ostream &os, const Field &rhs)
{
    os << "(" << rhs.x << "," << rhs.y << ",";
    return os;
}

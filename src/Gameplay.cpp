#include "Gameplay.hpp"

SDL_Point operator+(SDL_Point left, SDL_Point right)
{
    return {left.x + right.x, left.y + right.y};
}

SDL_Point operator-(SDL_Point left, SDL_Point right)
{
    return {left.x - right.x, left.y - right.y};
}

SDL_Point operator*(SDL_Point left, SDL_Point right)
{
    return {left.x * right.x, left.y * right.y};
}

SDL_Point operator/(SDL_Point left, SDL_Point right)
{
    return {(left.x / right.x), (left.x / right.y)};
}

SDL_Point operator*(double left, SDL_Point right)
{
    int x = (int) (left * right.x);
    int y = (int) (left * right.y);
    return {x, y};
}

SDL_Point operator*(SDL_Point left, double right)
{
    return right * left;
}

SDL_Point operator/(SDL_Point left, double right)
{
    return (1 / right) * left;
}

double operator!(SDL_Point left)
{
    double length = std::sqrt(left.x * left.x + left.y * left.y);
    return length;
}

Field Field::cubic_round(double x, double y, double z)
{
    double round_x = std::round(x);
    double round_y = std::round(y);
    double round_z = std::round(z);
    double x_err = std::abs(round_x - x);
    double y_err = std::abs(round_y - y);
    double z_err = std::abs(round_z - z);
    if (x_err > y_err && x_err > z_err)
    {
        round_x = -round_y - round_z;
    } else if (y_err > z_err)
    {
        round_y = -round_x - round_z;
    } else
    {
        round_z = -round_x - round_y;
    }
    Sint16 x_out = (int) round_x;
    Sint16 y_out = (int) round_y;
    Sint16 z_out = (int) round_z;
    return Field(x_out, y_out, z_out);
}


Field Field::hex_direction(Uint8 direction)
{
    assert (0 <= direction && direction <= 5);
    return hex_directions[direction];
}

Field Field::get_neighbor(Uint8 direction) const
{
    return hex_direction(direction) + *this;
}

Point Field::field_to_point(const Layout *layout) const
{
    const Orientation m = layout->orientation;
    double x = (m.f0 * this->x + m.f1 * this->y) * layout->size;
    double y = (m.f2 * this->x + m.f3 * this->y) * layout->size;
    return {x + layout->origin.x, y + layout->origin.y};
}

Field Point::point_to_field(const Layout *layout) const
{
    const Orientation m = layout->orientation;
    double rel_x = (this->x - layout->origin.x) / layout->size;
    double rel_y = (this->y - layout->origin.y) / layout->size;
    double x = m.b0 * rel_x + m.b1 * rel_y;
    double y = m.b2 * rel_x + m.b3 * rel_y;
    return Field::cubic_round(x, y, -x - y);
}

Point field_corner_offset(Uint8 corner, const Layout *layout)
{
    double angle = 2.0 * M_PI * (corner + layout->orientation.start_angle) / 6;
    double x = (layout->size * cos(angle));
    double y = (layout->size * sin(angle));
    return {x, y};
}

std::vector<Point> Field::field_to_polygon(const Layout *layout) const
{
    std::vector<Point> corners = this->field_to_polygon_normalized(layout);
    Point center = this->field_to_point(layout);
    for (Point &p : corners)
    {
        p = p + center;
    }
    return corners;
}

std::vector<Point> Field::field_to_polygon_normalized(const Layout *layout) const
{
    std::vector<Point> corners;
    for (uint8_t i = 0; i < 6; i++)
    {
        Point offset = field_corner_offset(i, layout);
        corners.push_back(offset);
    }
    return corners;
}

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
    if (this->owner->get_id().is_nil())
        color = {0x77, 0x77, 0x77, 0x77};
    filledPolygonRGBA(renderer, vx, vy, 6, color.r, color.g, color.b, 0x22);
    SDL_Color inverse;
    inverse.r = (Uint8) (color.r + 0x77);
    inverse.g = (Uint8) (color.g + 0x77);
    inverse.b = (Uint8) (color.b + 0x77);
    inverse.a = 0xff;
    double resource_size = layout->size / 4;
    if (this->resources_base.triangle > 0)
    {
        static const SDL_Point trigon[] = {{0,  -1},
                                           {-1, 1},
                                           {1,  1}};
        for (int i = 0; i < 3; i++)
        {
            vx[i] = (Sint16) (location.x + (trigon[i].x * resource_size));
            vy[i] = (Sint16) (location.y + (trigon[i].y * resource_size));
        }
        trigonRGBA(renderer, vx[0], vy[0], vx[1], vy[1], vx[2], vy[2], inverse.r, inverse.g, inverse.b, inverse.a);
    }
    if (this->resources_base.circle > 0)
    {
        circleRGBA(renderer, (Sint16) (location.x), Sint16(location.y), (Sint16) resource_size, inverse.r, inverse.g,
                   inverse.b, inverse.a);
    }
    if (this->resources_base.square > 0)
    {
        static const SDL_Point square[] = {{-1, -1},
                                           {-1, 1},
                                           {1,  1},
                                           {1,  -1}};
        for (int i = 0; i < 4; i++)
        {
            vx[i] = (Sint16) (location.x + square[i].x * resource_size);
            vy[i] = (Sint16) (location.y + square[i].y * resource_size);
        }
        polygonRGBA(renderer, vx, vy, 4, inverse.r, inverse.g, inverse.b, inverse.a);
    }
}

void FieldMeta::trigger_event(Uint32 type, Sint32 code)
{
    SDL_Event event;
    SDL_memset(&event, 0, sizeof(event)); /* or SDL_zero(event) */
    event.type = type;
    event.user.code = code;
    event.user.data1 = static_cast<void *>(this);
    event.user.data2 = 0;
    SDL_PushEvent(&event);
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
    this->trigger_event(BOB_FIELDUPDATEEVENT, 0);
}

Resource HexagonGrid::get_resources_of_cluster(Cluster *cluster)
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
    if (this->upgrades[upgrade])
        return this->upgrades[upgrade];
    Cluster cluster = this->grid->get_cluster(this);
    Resource cluster_resources = this->grid->get_resources_of_cluster(&cluster);
    auto pair = UPGRADE_COSTS.find(upgrade);
    if (pair != UPGRADE_COSTS.end())
    {
        Resource costs = pair->second;
        if (costs > cluster_resources) // too expensive for you
            return this->upgrades[upgrade];
        Resource remaining_costs = this->grid->consume_resources_of_cluster(&cluster, costs);
        static const Resource neutral = {0, 0, 0};
        if (remaining_costs == neutral)
        {
            this->upgrades[upgrade] = true;
        }
    }
    this->trigger_event(BOB_FIELDUPGRADEVENT, 0);
    return this->upgrades[upgrade];
}

FieldMeta *FieldMeta::get_neighbor(Uint8 direction)
{
    return this->grid->get_neighbor(this, direction);
}

FieldMeta *HexagonGrid::get_neighbor(FieldMeta *meta, Uint8 direction)
{
    Field neighbor_field = meta->get_field().get_neighbor(direction);
    FieldMeta *neighbor = nullptr;
    auto pair = this->fields.find(neighbor_field);
    if (pair != this->fields.end())
        neighbor = pair->second;
    return neighbor;
}

Cluster HexagonGrid::get_cluster(FieldMeta *field)
{
    Cluster visited = Cluster();
    FieldMeta *current = nullptr;
    std::set<FieldMeta *> seen = {field};
    while (!seen.empty()) // still unvisited fields, that can be reached
    {
        current = *(seen.begin());
        seen.erase(current);
        for (Uint8 i = 0; i < 6; i++)
        {
            FieldMeta *neighbor = this->get_neighbor(current, i);
            // neighbor is unvisited, unseen and inside same cluster
            if (neighbor != nullptr && *(neighbor->get_owner()) == *(current->get_owner())
                && visited.find(neighbor) == visited.end() && seen.find(neighbor) == seen.end())
            {
                seen.insert(neighbor); // discovered an unseen neighbor, will visit later
            }
        }
        visited.insert(current); // have seen all of the neighbors for this field
    }
    return visited;
}

void FieldMeta::consume_resources(Resource costs)
{
    this->resources -= costs;
    this->trigger_event(BOB_FIELDUPDATEEVENT, 0);
}

Resource HexagonGrid::consume_resources_of_cluster(Cluster *cluster, Resource costs)
{
    for (FieldMeta *meta : *cluster)
    {
        // mind the "special" definition of -=, only byte of what you can chew or leave nothing behind
        Resource tmp = costs;
        costs -= meta->get_resources();
        meta->consume_resources(tmp);
    }
    return costs; // > {0, 0, 0} means there were not enough resources
}

bool Player::fight(FieldMeta *field)
{
    if (*this == *(field->get_owner())) // attacked field outside of the map or friendly fire
    {
        return false;
    }
    if (field->get_owner()->get_id().is_nil()) // still to be had
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
    if (event->type == BOB_NEXTTURNEVENT)
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
            filledPolygonRGBA(renderer, x, y, 6, 0x77, 0x77, 0x77, 0x22);
        }
        elem.second->render(renderer, this->layout);
    }
    return true;
}

void HexagonGrid::handle_event(SDL_Event *event)
{
    SDL_Point mouse = {0, 0};
    SDL_GetMouseState(&mouse.x, &mouse.y);
    int scroll = this->layout->size / 10 * event->wheel.y;
    double old_size = this->layout->size;
    SDL_Point old_origin = this->layout->origin;
    switch (event->type)
    {
        case SDL_MOUSEWHEEL:
            if (old_size + scroll < 20)
            {
                this->layout->size = 20;
            }
            else if (old_size + scroll > 1000)
            {
                this->layout->size = 1000;
            }
            else
            {
                this->layout->size += scroll;
            }
            this->move(((1.0 - (double) this->layout->size / old_size) * (mouse - old_origin)));
            if (!on_rectangle(&layout->box))
                this->layout->size -= scroll;
            break;
        case SDL_MOUSEMOTION:
            if (this->panning)
            {
                Point marker_pos = this->marker->get_field().field_to_point(this->layout);
                SDL_Point p;
                p.x = (int) marker_pos.x;
                p.y = (int) marker_pos.y;
                this->move(mouse - p);
            }
            this->update_marker();
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event->button.button)
            {
                case SDL_BUTTON_MIDDLE:
                    this->panning = !(this->panning);
                    break;
                case SDL_BUTTON_RIGHT:
                    this->marker->trigger_event(BOB_FIELDSELECTED, 0);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    for (auto elem : this->fields)
    {
        elem.second->handle_event(event);
    }
}

void HexagonGrid::move(SDL_Point m)
{
    this->layout->origin = this->layout->origin + m;
    // check if some part is inside layout->box
    if (!on_rectangle(&layout->box))
        this->layout->origin = this->layout->origin - m;
    this->update_marker();
}

void HexagonGrid::update_marker()
{
    if (!Timer::MOUSE_LOCKED)
    {
        SDL_Point m = {0, 0};
        SDL_GetMouseState(&(m.x), &(m.y));
        Point p = {0.0, 0.0};
        p.x = m.x;
        p.y = m.y;
        FieldMeta *n_marker = this->point_to_field(p);
        if (n_marker != nullptr)
        {
            this->marker = n_marker;
            n_marker->trigger_event(BOB_MARKERUPDATE, 0);
        }
        else
        {
            marker->trigger_event(BOB_MARKERUPDATE, 1);
        }
    }
}

FieldMeta *HexagonGrid::point_to_field(const Point p)
{
    Field field = p.point_to_field(this->layout);
    FieldMeta *meta = nullptr;
    auto pair = this->fields.find(field);
    if (pair != this->fields.end())
        meta = pair->second;
    return meta;

}

FieldMeta *HexagonGrid::get_field(Field field)
{
    FieldMeta *meta = nullptr;
    auto pair = this->fields.find(field);
    if (pair != this->fields.end())
        meta = pair->second;
    return meta;
}

bool HexagonGrid::on_rectangle(SDL_Rect *rect)
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

void HexagonGrid::update_dimensions(SDL_Point dimensions)
{
    this->layout->origin = {dimensions.x / 2, dimensions.y / 2};
    this->layout->box.w = dimensions.x;
    this->layout->box.h = dimensions.y;
}

Point HexagonGrid::field_to_point(FieldMeta *field)
{
    return field->get_field().field_to_point(this->layout);
}
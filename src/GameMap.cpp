#include "GameMap.hpp"

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

SDL_Point operator*(int left, SDL_Point right)
{
    return {left * right.x, left * right.y};
}

SDL_Point operator*(SDL_Point left, int right)
{
    return right * left;
}

SDL_Point operator/(SDL_Point left, int right)
{
    return {left.x / right, left.y / right};
}

int operator!(SDL_Point left)
{
    int length = (int) std::sqrt(left.x * left.x + left.y * left.y);
    return length;
}

Point operator+(Point left, Point right)
{
    return Point(left.x + right.x, left.y + right.y);
}

Point operator-(Point left, Point right)
{
    return Point(left.x - right.x, left.y - right.y);
}

Point operator*(Point left, Point right)
{
    return Point(left.x * right.x, left.y * right.y);
}

Point operator/(Point left, Point right)
{
    return Point(left.x / right.x, left.y / right.y);
}

Point operator*(double left, Point right)
{
    return Point(left * right.x, left * right.y);
}

Point operator*(Point left, double right)
{
    return Point(left.x * right, left.y * right);
}

Point operator/(double left, Point right)
{
    return Point(left * right.x, left * right.y);
}

Point operator/(Point left, double right)
{
    return Point(left.x * right, left.y * right);
}

double operator!(Point left)
{
    return std::sqrt(left.x * left.x + left.y * left.y);
}

bool operator==(Field left, Field right)
{
    return left.x == right.x && left.y == right.y;
}

bool operator!=(Field left, Field right)
{
    return left == right;
}

Field operator+(Field left, Field right)
{
    return Field(left.x + right.x, left.y + right.y, left.z + right.z);
}

Field operator-(Field left, Field right)
{
    return Field(left.x - right.x, left.y - right.y, left.z - right.z);
}

Field operator*(Field left, Field right)
{
    return Field(left.x * right.x, left.y * right.y, left.z * right.z);
}

Field operator/(Field left, Field right)
{
    return cubic_round(left.x / right.x, left.y / right.y, right.z / left.z);
}

Field cubic_round(double x, double y, double z)
{
    Sint8 round_x = (Sint8) std::round(x);
    Sint8 round_y = (Sint8) std::round(y);
    Sint8 round_z = (Sint8) std::round(z);
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
    return Field(round_x, round_y, round_z);
}

int cubic_distance(Field a, Field b)
{
    return (abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z)) / 2;
}

Field hex_direction(int direction)
{
    assert (0 <= direction && direction <= 5);
    return hex_directions[direction];
}

Field hex_neighbor(int8_t direction, Field f)
{
    return hex_direction(direction) + f;
}

Point field_to_point(const Field f, const Layout layout)
{
    const Orientation m = layout.orientation;
    double x = (m.f0 * f.x + m.f1 * f.y) * layout.size;
    double y = (m.f2 * f.x + m.f3 * f.y) * layout.size;
    return Point(x + layout.origin.x, y + layout.origin.y);
}

Field point_to_field(Point point, const Layout layout)
{
    const Orientation m = layout.orientation;
    double rel_x = (point.x - layout.origin.x) / layout.size;
    double rel_y = (point.y - layout.origin.y) / layout.size;
    double x = m.b0 * rel_x + m.b1 * rel_y;
    double y = m.b2 * rel_x + m.b3 * rel_y;
    return cubic_round(x, y, -x - y);
}

Point field_corner_offset(uint8_t corner, const Layout layout)
{
    double angle = 2.0 * M_PI * (corner + layout.orientation.start_angle) / 6;
    return Point(layout.size * cos(angle), layout.size * sin(angle));
}

std::vector<Point> field_to_polygon(const Field field, const Layout layout)
{
    std::vector<Point> corners = {};
    Point center = field_to_point(field, layout);
    for (uint8_t i = 0; i < 6; i++)
    {
        Point offset = field_corner_offset(i, layout);
        corners.push_back(Point(center.x + offset.x, center.y + offset.y));
    }
    return corners;
}

HexagonGrid::HexagonGrid(Sint16 grid_radius, Layout layout)
        : Grid(layout)
{
    // first lower half, then upper half
    for (Sint16 x = -grid_radius; x <= grid_radius; x++)
    {
        Sint16 y_l = (-grid_radius > -x - grid_radius) ? -grid_radius : -x - grid_radius;
        Sint16 y_u = (grid_radius < -x + grid_radius) ? grid_radius : -x + grid_radius;
        for (Sint16 y = y_l; y <= y_u; y++)
        {
            Sint16 z = -x - y;
            Field new_field = {x, y, z};
            FieldMeta *meta = new FieldMeta(nullptr);
            this->fields.insert(new_field);
            std::pair<Field, FieldMeta *> field_pair(new_field, meta);
            this->fields_meta.insert(field_pair);
        }
    }
}

bool HexagonGrid::render(SDL_Renderer *renderer)
{
    Field some_field = {0, 0, 0};
    std::vector<Point> polygon = field_to_polygon(some_field, this->layout);
    Point center = field_to_point(some_field, this->layout);
    assert(polygon.size() > 5);
    Sint16 vx[6];
    Sint16 vy[6];
    for (uint8_t i = 0; i < 6; i++)
    {
        vx[i] = (Sint16) (polygon[i].x - center.x);
        vy[i] = (Sint16) (polygon[i].y - center.y);
    }
    for (const Field &elem : this->fields)
    {
        center = field_to_point(elem, this->layout);
        Sint16 x[6];
        Sint16 y[6];
        for (uint8_t i = 0; i < 6; i++)
        {
            x[i] = vx[i] + (Sint16) center.x;
            y[i] = vy[i] + (Sint16) center.y;
        }
        polygonRGBA(renderer, x, y, 6, 0xff, 0xff, 0xff, 0xff);
        if (elem == this->marker)
        {
            filledPolygonRGBA(renderer, x, y, 6, 0xff, 0x0, 0x0, 0xff);
        }
    }
    return true;
}

void Grid::move(SDL_Point move)
{
    this->set_origin(this->layout.origin + move);
}

void Grid::handle_event(SDL_Event *event)
{

    if (event->type == SDL_MOUSEWHEEL)
    {
        SDL_Point mouse = {0, 0};
        SDL_GetMouseState(&mouse.x, &mouse.y);
        int scroll = event->wheel.y;
        Uint16 old_size = this->layout.size;
        SDL_Point old_origin = this->layout.origin;

        if (old_size + scroll < 10)
        {
            this->layout.size = 10;
        }
        else if (old_size + scroll > 100)
        {
            this->layout.size = 100;
        }
        else
        {
            this->layout.size += scroll;
            this->set_origin(old_origin + (10 * scroll * (old_origin - mouse) / !(old_origin - mouse)));
        }
    }
    if (event->type == SDL_MOUSEMOTION)
    {
        this->update_marker();
    }
}

void Grid::set_origin(SDL_Point dm)
{
    this->layout.origin = dm;
    this->update_marker();
}

void Grid::update_marker()
{
    SDL_Point m;
    SDL_GetMouseState(&(m.x), &(m.y));
    Point p = {0.0, 0.0};
    p.x = m.x;
    p.y = m.y;
    this->marker = point_to_field(p, this->layout);
}

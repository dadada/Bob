#include "Grid.hpp"

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

Field hex_direction(Uint8 direction)
{
    assert (0 <= direction && direction <= 5);
    return hex_directions[direction];
}

Field hex_neighbor(Uint8 direction, Field f)
{
    return hex_direction(direction) + f;
}

Point field_to_point(const Field f, const Layout *layout)
{
    const Orientation m = layout->orientation;
    double x = (m.f0 * f.x + m.f1 * f.y) * layout->size;
    double y = (m.f2 * f.x + m.f3 * f.y) * layout->size;
    return {x + layout->origin.x, y + layout->origin.y};
}

Field point_to_field(Point point, const Layout *layout)
{
    const Orientation m = layout->orientation;
    double rel_x = (point.x - layout->origin.x) / layout->size;
    double rel_y = (point.y - layout->origin.y) / layout->size;
    double x = m.b0 * rel_x + m.b1 * rel_y;
    double y = m.b2 * rel_x + m.b3 * rel_y;
    return cubic_round(x, y, -x - y);
}

Point field_corner_offset(Uint8 corner, const Layout *layout)
{
    double angle = 2.0 * M_PI * (corner + layout->orientation.start_angle) / 6;
    double x = (layout->size * cos(angle));
    double y = (layout->size * sin(angle));
    return {x, y};
}

std::vector<Point> field_to_polygon(const Field field, const Layout *layout)
{
    std::vector<Point> corners = field_to_polygon_normalized(field, layout);
    Point center = field_to_point(field, layout);
    for (Point &p : corners)
    {
        p = p + center;
    }
    return corners;
}

std::vector<Point> field_to_polygon_normalized(const Field field, const Layout *layout)
{
    std::vector<Point> corners;
    for (uint8_t i = 0; i < 6; i++)
    {
        Point offset = field_corner_offset(i, layout);
        corners.push_back(offset);
    }
    return corners;
}

HexagonGrid::HexagonGrid(Sint16 grid_radius, Layout *layout)
        : Grid(layout), radius(grid_radius)
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
            this->fields->insert(new_field);
        }
    }
}

bool HexagonGrid::render(SDL_Renderer *renderer)
{
    Field some_field = {0, 0, 0};
    std::vector<Point> polygon = field_to_polygon_normalized(some_field, this->layout);
    assert(polygon.size() > 5);
    Sint16 x[6];
    Sint16 y[6];
    for (const Field &elem : *(this->fields))
    {
        Point center = field_to_point(elem, this->layout);
        for (uint8_t i = 0; i < 6; i++)
        {
            x[i] = (Sint16) (center.x + polygon[i].x);
            y[i] = (Sint16) (center.y + polygon[i].y);
        }
        const SDL_Color color = {0xff, 0xff, 0xff, 0xff};
        polygonRGBA(renderer, x, y, 6, color.r, color.g, color.b, color.a);
        if (elem == this->marker)
        {
            filledPolygonRGBA(renderer, x, y, 6, 0x77, 0x77, 0x77, 0x77);
        }
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
        this->update_marker();
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
    this->marker = point_to_field(p, this->layout);
}

bool Grid::on_rectangle(SDL_Rect *rect)
{
    // check if center inside rect for ANY field
    for (const Field &f : *(this->fields))
    {
        Point precise_p = field_to_point(f, layout);
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

void Grid::update_box(int x, int y)
{
    this->layout->box.w = x;
    this->layout->box.h = y;
}
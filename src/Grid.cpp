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

Field Field::cubic_round(double x, double y, double z)
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

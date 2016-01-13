#include "GameMap.hpp"

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
    int8_t round_x = (int8_t) std::round(x);
    int8_t round_y = (int8_t) std::round(y);
    int8_t round_z = (int8_t) std::round(z);
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

HexagonGrid::HexagonGrid(Sint16 grid_radius, Layout layout, SDL_Color color)
        : Grid(layout, color)
{
    // first lower half, then upper half
    for (Sint16 x = -grid_radius; x <= grid_radius; x++)
    {
        Sint16 y_l = (-grid_radius > -x - grid_radius) ? -grid_radius : -x - grid_radius;
        Sint16 y_u = (grid_radius < -x + grid_radius) ? grid_radius : -x + grid_radius;
        for (Sint16 y = y_l; y <= y_u; y++)
        {
            Sint16 z = -x - y;
            this->fields.insert({x, y, z});
        }
    }
}

bool HexagonGrid::render(SDL_Renderer *renderer)
{
    for (const Field &elem : this->fields)
    {
        std::vector<Point> polygon = field_to_polygon(elem, this->layout);
        assert(polygon.size() > 5);
        Sint16 vx[6];
        Sint16 vy[6];
        for (uint8_t i = 0; i < 6; i++)
        {
            vx[i] = (Sint16) polygon[i].x;
            vy[i] = (Sint16) polygon[i].y;
        }
        const Sint16 x[6] = {vx[0], vx[1], vx[2], vx[3], vx[4], vx[5]};
        const Sint16 y[6] = {vy[0], vy[1], vy[2], vy[3], vy[4], vy[5]};
        aapolygonRGBA(renderer, x, y, 6, this->color.r, this->color.g, this->color.b, this->color.a);
    }
    return true;
}

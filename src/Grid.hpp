#include <iostream>
#include <stdexcept>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cmath>
#include <vector>
#include <assert.h>
#include <unordered_set>

#ifndef POINT_OPERATORS
#define POINT_OPERATORS
SDL_Point operator+(SDL_Point left, SDL_Point right);

SDL_Point operator-(SDL_Point left, SDL_Point right);

SDL_Point operator*(SDL_Point left, SDL_Point right);

SDL_Point operator/(SDL_Point left, SDL_Point right);

SDL_Point operator/(SDL_Point left, SDL_Point right);

SDL_Point operator*(double left, SDL_Point right);

SDL_Point operator*(SDL_Point left, double right);

SDL_Point operator/(SDL_Point left, double right);

double operator!(SDL_Point left);

#endif

#ifndef Point
struct Point
{
    double x;
    double y;

    Point(double x_, double y_) : x(x_), y(y_)
    { }
};

#endif

Point operator+(Point left, Point right);

Point operator-(Point left, Point right);

Point operator*(Point left, Point right);

Point operator/(Point left, Point right);

Point operator*(double left, Point right);

Point operator*(Point left, double right);

Point operator/(double left, Point right);

Point operator/(Point left, double right);

double operator!(Point left);

#ifndef Orientation
struct Orientation
{
    // cubic to point
    const double f0, f1, f2, f3;
    // point to cubic
    const double b0, b1, b2, b3;
    // in multiples of 60 deg
    const double start_angle;

    Orientation(double f0_, double f1_, double f2_, double f3_, double b0_, double b1_, double b2_, double b3_,
                double start_angle_)
            : f0(f0_), f1(f1_), f2(f2_), f3(f3_), b0(b0_), b1(b1_), b2(b2_), b3(b3_),
              start_angle(start_angle_)
    { }
};

const Orientation pointy_orientation = Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0,
                                                   sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0,
                                                   0.5);
const Orientation flat_orientation = Orientation(3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0),
                                                 2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0,
                                                 0);
#endif

#ifndef Layout

struct Layout
{
    const Orientation orientation;
    Uint16 size;
    SDL_Point origin;
    SDL_Rect box;

    Layout(Orientation orientation_, Uint16 size_, SDL_Point origin_, SDL_Rect box_)
            : orientation(orientation_), size(size_), origin(origin_), box(box_)
    { }
};

#endif

#ifndef Field
struct Field
{
    Sint16 x, y, z;

    Field(Sint16 x_, Sint16 y_, Sint16 z_) : x(x_), y(y_), z(z_)
    {
        assert(x + y + z == 0);
    }
};

// from upper right corner
const std::vector<Field> hex_directions = {
        Field(1, 0, -1), Field(0, 1, -1), Field(-1, 1, 0), Field(-1, 0, 1), Field(-1, 1, 0)
};

Field hex_direction(Uint8 direction);

namespace std
{
    template<>
    struct hash<Field>
    {
        size_t operator()(const Field &f) const
        {
            hash<Sint16> int_hash;
            size_t hx = int_hash(f.x);
            size_t hy = int_hash(f.y);
            // hz would be redundant, since f.z is redundant
            // combine hashes
            return hx ^ (hy + 0x9e3779b9 + (hx << 6) + (hx >> 2));
        }
    };
}

Field hex_neighbor(Uint8 direction, Field f);

Field cubic_round(double x, double y, double z);

int cubic_distance(Field a, Field b);

bool operator==(Field left, Field right);

bool operator!=(Field left, Field right);

Field operator+(Field left, Field right);

Field operator-(Field left, Field right);

Field operator*(Field left, Field right);

Field operator/(Field left, Field right);
#endif

Point field_to_point(const Field f, const Layout *layout);

Field point_to_field(Point point, const Layout *layout);

Point field_corner_offset(Uint8 corner, const Layout *layout);

std::vector<Point> field_to_polygon_normalized(const Field field, const Layout *layout);

std::vector<Point> field_to_polygon(const Field field, const Layout *layout);


#ifndef Grid

class Grid
{
protected:
    std::unordered_set<Field> *fields;
    Layout *layout;
    Field marker;
    bool panning;
    bool on_rectangle(SDL_Rect *rect);
public:
    Grid(Layout *layout_)
            : layout(layout_), marker(0, 0, 0)
    {
        this->fields = new std::unordered_set<Field>();
    };

    ~Grid()
    {
        delete this->fields;
    }

    std::unordered_set<Field> *get_fields() { return this->fields; }

    void move(SDL_Point move);

    void update_marker();

    virtual bool render(SDL_Renderer *renderer) = 0;

    void handle_event(SDL_Event *event);

    void update_box(int x, int y);
};

#endif

#ifndef HexagonGrid
class HexagonGrid : public Grid
{
private:
    Sint16 radius;
public:
    HexagonGrid(Sint16 grid_radius, Layout *layout);

    bool render(SDL_Renderer *renderer);

    Sint16 get_radius() { return radius * layout->size; }
};

#endif

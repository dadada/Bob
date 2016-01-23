#include <iostream>
#include <stdexcept>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cmath>
#include <vector>
#include <assert.h>
#include <unordered_set>

#ifndef _GRID_H
#define _GRID_H

SDL_Point operator+(SDL_Point left, SDL_Point right);

SDL_Point operator-(SDL_Point left, SDL_Point right);

SDL_Point operator*(SDL_Point left, SDL_Point right);

SDL_Point operator/(SDL_Point left, SDL_Point right);

SDL_Point operator/(SDL_Point left, SDL_Point right);

SDL_Point operator*(double left, SDL_Point right);

SDL_Point operator*(SDL_Point left, double right);

SDL_Point operator/(SDL_Point left, double right);

double operator!(SDL_Point left);

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

const Orientation pointy_orientation = Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0, sqrt(3.0) / 3.0,
                                                   -1.0 / 3.0, 0.0, 2.0 / 3.0, 0.5);
const Orientation flat_orientation = Orientation(3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0), 2.0 / 3.0, 0.0, -1.0 / 3.0,
                                                 sqrt(3.0) / 3.0, 0);

struct Layout
{
    const Orientation orientation;
    Sint16 size;
    SDL_Point origin;
    SDL_Rect box;

    Layout(Orientation orientation_, Sint16 size_, SDL_Point origin_, SDL_Rect box_)
            : orientation(orientation_), size(size_), origin(origin_), box(box_)
    { }
};

struct Field;

struct Point
{
    double x;
    double y;

    Point(double x_, double y_) : x(x_), y(y_) { }

    bool operator==(const Point &rhs) const
    {
        return (this->x == rhs.x && this->y == rhs.y);
    }

    inline bool operator!=(const Point &rhs) const
    {
        return !(*this == rhs);
    }

    Point &operator+=(const Point &rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

    friend Point operator+(Point lhs, const Point &rhs)
    {
        lhs += rhs;
        return lhs;
    }

    Point &operator-=(const Point &rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }

    friend Point operator-(Point lhs, const Point &rhs)
    {
        lhs -= rhs;
        return rhs;
    }

    Point &operator*=(const Point &rhs)
    {
        this->x *= rhs.x;
        this->y *= rhs.y;
        return *this;
    }

    friend Point operator*(Point lhs, const Point &rhs)
    {
        lhs *= rhs;
        return rhs;
    }

    Point &operator/=(const Point &rhs)
    {
        this->x /= rhs.x;
        this->y /= rhs.y;
        return *this;
    }

    friend Point operator/(Point lhs, const Point &rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    Point &operator*=(const double &rhs)
    {
        this->x *= rhs;
        this->y *= rhs;
        return *this;
    }

    friend Point operator*(Point lhs, const double &rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    Point &operator/=(const double &rhs)
    {
        this->x /= rhs;
        this->y /= rhs;
        return *this;
    }

    friend Point operator/(Point lhs, const double &rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    double operator!()
    {
        return std::sqrt(this->x * this->x + this->y * this->y);
    }

    Field point_to_field(const Layout *layout) const;
};

struct Field
{
    Sint16 x, y, z;

    Field(Sint16 x_, Sint16 y_, Sint16 z_) : x(x_), y(y_), z(z_)
    {
        assert(x + y + z == 0);
    }

    bool operator==(const Field &rhs) const
    {
        return (this->x == rhs.x && this->y == rhs.y);
    }

    inline bool operator!=(const Field &rhs) const
    {
        return !(*this == rhs);
    }

    Field &operator+=(const Field &rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }

    friend Field operator+(Field lhs, const Field &rhs)
    {
        lhs += rhs;
        return lhs;
    }

    Field &operator-=(const Field &rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        return *this;
    }

    friend Field operator-(Field lhs, const Field &rhs)
    {
        lhs -= rhs;
        return rhs;
    }

    Field &operator*=(const Field &rhs)
    {
        this->x *= rhs.x;
        this->y *= rhs.y;
        this->z *= rhs.z;
        return *this;
    }

    friend Field operator*(Field lhs, const Field &rhs)
    {
        lhs *= rhs;
        return rhs;
    }

    Field &operator/=(const Field &rhs)
    {
        double x = this->x / rhs.x;
        double y = this->y / rhs.y;
        double z = this->z / rhs.z;
        Field f = cubic_round(x, y, z);
        *this = f;
        return *this;
    }

    friend Field operator/(Field lhs, const Field &rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    Field &operator*=(const double &rhs)
    {
        double x = this->x * rhs;
        double y = this->y * rhs;
        double z = this->z * rhs;
        Field f = cubic_round(x, y, z);
        *this = f;
        return *this;
    }

    friend Field operator*(Field lhs, const double &rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    Field &operator/=(const double &rhs)
    {
        double x = this->x / rhs;
        double y = this->y / rhs;
        double z = this->z / rhs;
        Field f = cubic_round(x, y, z);
        *this = f;
        return *this;
    }

    friend Field operator/(Field lhs, const double &rhs)
    {
        lhs /= rhs;
        return lhs;
    }

    int operator&(const Field &rhs)
    {
        return (abs(this->x - rhs.x) + abs(this->y - rhs.y) + abs(this->z - rhs.z)) / 2;
    }

    friend std::ostream &operator<<(std::ostream &os, const Field &rhs);

    Field get_neighbor(Uint8 direction) const;

    Point field_to_point(const Layout *layout) const;

    std::vector<Point> field_to_polygon_normalized(const Layout *layout) const;

    std::vector<Point> field_to_polygon(const Layout *layout) const;

    static Field cubic_round(double x, double y, double z);

    static Field hex_direction(Uint8 direction);
};

// from upper right corner
const std::vector<Field> hex_directions = {Field(1, 0, -1), Field(0, 1, -1), Field(-1, 1, 0), Field(-1, 0, 1),
                                           Field(-1, 1, 0)};

Point field_corner_offset(Uint8 corner, const Layout *layout);

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

#endif

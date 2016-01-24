#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

#include <random>
#include <iostream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <assert.h>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "Events.hpp"

#define NUM_UPGRADES 10

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
              start_angle(start_angle_) { }
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
            : orientation(orientation_), size(size_), origin(origin_), box(box_) { }
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

inline std::ostream &operator<<(std::ostream &os, const Field &rhs)
{
    os << "(" << rhs.x << "," << rhs.y << ",";
    return os;
}

typedef std::bitset<NUM_UPGRADES> UpgradeFlags;

struct Resource
{
    Uint8 circle;
    Uint8 triangle;
    Uint8 square;

    Resource &operator+=(const Resource &rhs)
    {
        this->circle += rhs.circle;
        this->triangle += rhs.triangle;
        this->square += rhs.square;
        return *this;
    }

    friend Resource operator+(Resource lhs, const Resource &rhs)
    {
        lhs += rhs;
        return lhs;
    }

    Resource &operator-=(const Resource &rhs)
    {
        if (this->circle < rhs.circle)
            this->circle = 0;
        else
            this->circle -= rhs.circle;

        if (this->triangle < rhs.triangle)
            this->triangle = 0;
        else
            this->triangle -= rhs.triangle;

        if (this->square < rhs.square)
            this->square = 0;
        else
            this->square -= rhs.square;

        return *this;
    }

    friend Resource operator-(Resource lhs, const Resource &rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    Resource &operator*=(const Resource &rhs)
    {
        this->circle *= rhs.circle;
        this->triangle *= rhs.triangle;
        this->square *= rhs.square;
        return *this;
    }

    friend Resource operator*(Resource lhs, const Resource &rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    Resource &operator*=(const double rhs)
    {
        this->circle *= rhs;
        this->triangle *= rhs;
        this->square *= rhs;
        return *this;
    }

    friend Resource operator*(const double lhs, Resource rhs)
    {
        rhs *= lhs;
        return rhs;
    }

    friend Resource operator*(Resource lhs, const double rhs)
    {
        return rhs * lhs;
    }

    bool operator<(const Resource &rhs) const
    {
        return (this->circle < rhs.circle || this->triangle < rhs.triangle || this->square < rhs.square);
    }

    inline bool operator>(const Resource &rhs) const { return rhs < *this; }

    inline bool operator<=(const Resource &rhs) const { return !(*this > rhs); }

    inline bool operator>=(const Resource &rhs) const { return !(*this < rhs); }

    bool operator==(const Resource &rhs) const
    {
        return (this->circle == rhs.circle && this->triangle == rhs.triangle && this->square == rhs.square);
    }

    inline bool operator!=(const Resource &rhs) const { return !(*this == rhs); }
};

enum Upgrade
{
    First_Upgrade = 0,
    Regeneration_1,
    Regeneration_2,
    Regeneration_3,
    Reproduction_1,
    Reproduction_2,
    Reproduction_3
};

namespace std
{
    template<>
    struct hash<Upgrade>
    {
        size_t operator()(const Upgrade &f) const
        {
            int i = static_cast<int>(f);
            hash<int> int_hash;
            return int_hash(f);
        }
    };
}

const std::unordered_map<Upgrade, Resource> UPGRADE_COSTS(
        {
                {Regeneration_1, {4,  4,  4}},
                {Regeneration_2, {8,  8,  8}},
                {Regeneration_3, {16, 16, 16}},
                {Reproduction_1, {4,  4,  4}},
                {Reproduction_2, {8,  8,  8}},
                {Reproduction_3, {16, 16, 16}}
        }
);

class FieldMeta;

class Player
{

public:
    Player()
            : name("Default Player"), uuid(boost::uuids::nil_uuid()) { }
    Player(std::string name_)
            : name(name_), uuid(boost::uuids::basic_random_generator<boost::mt19937>()())
    {
        // use the last 24 bits of the tag for the color
        boost::uuids::uuid id = this->uuid;
        uint8_t *data = id.data;
        this->color = {data[13], data[14], data[15], 0xff};
    }

    SDL_Color get_color() { return this->color; }

    std::string get_name()
    {
        std::ostringstream descriptor;
        boost::uuids::uuid id = this->uuid;
        uint8_t *data = id.data;
        Uint16 number = (data[14] << 8) | (data[15]);
        descriptor << this->name << " (" << std::hex << number << ")";
        return descriptor.str();
    }

    bool fight(FieldMeta *field);

    boost::uuids::uuid get_id() { return this->uuid; }

    bool operator==(const Player &rhs) const
    {
        return rhs.uuid == this->uuid;
    }

private:
    boost::uuids::uuid uuid;
    SDL_Color color;
    std::string name;
};

class Grid;

class HexagonGrid;

class FieldMeta
{
public:
    FieldMeta(Field field_, Player *owner_)
            : field(field_), owner(owner_)
    {
        this->upgrades = 0;
        static std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<Uint8> distro(0, 1);
        this->resources_base.circle = distro(rng);
        this->resources_base.triangle = distro(rng);
        this->resources_base.square = distro(rng);
        this->offense = 1;
        this->defense = 1;
    }

    HexagonGrid *get_grid() { return this->grid; }

    int get_offense() { return this->offense; }

    int get_defense() { return this->defense; }

    Field get_field() { return this->field; }

    Player *get_owner() { return this->owner; }

    void set_owner(Player *player) { this->owner = player; }

    void render(SDL_Renderer *renderer, Layout *layout);

    Resource get_resources() { return this->resources; }

    UpgradeFlags get_upgrades() { return this->upgrades; }

    void consume_resources(Resource costs) { this->resources -= costs; }

    void regenerate_resources();

    bool upgrade(Upgrade upgrade);

    void handle_event(const SDL_Event *event, EventContext *context);

    FieldMeta *get_neighbor(Uint8 direction);

private:
    const Field field;
    HexagonGrid *grid;
    Player *owner;
    UpgradeFlags upgrades;
    Resource resources_base; // without upgrades applied, used as basis of regeneration
    Resource resources; // actual current resources
    int offense;
    int defense;
};

typedef std::unordered_set<FieldMeta *> Cluster;

class Grid
{

public:
    Grid(Layout *layout_)
            : layout(layout_)
    {
        Field f = {0, 0, 0};
        std::unordered_map<Field, FieldMeta *> fields = std::unordered_map<Field, FieldMeta *>();
        this->default_player = new Player();
        this->marker = new FieldMeta(f, this->default_player);
    };

    ~Grid()
    {
        for (auto const &elem : this->fields)
        {
            delete elem.second;
        }
        delete this->default_player;
    }

    void move(SDL_Point move);

    void update_marker();

    virtual bool render(SDL_Renderer *renderer) { return false; };

    void handle_event(SDL_Event *event, EventContext *context);

    void update_box(SDL_Point dimensions);

    Resource get_resources_of_cluster(const Cluster *cluster);

    virtual FieldMeta *get_neighbor(FieldMeta *field, Uint8 direction) = 0;

    virtual Cluster *get_cluster(FieldMeta *field, Cluster *visited = nullptr) = 0;

    Resource consume_resources_of_cluster(Cluster *cluster, Resource costs);

    FieldMeta *point_to_field(const Point p);

protected:
    std::unordered_map<Field, FieldMeta *> fields;
    Layout *layout;
    FieldMeta *marker;
    bool panning;
    Player *default_player;
    bool on_rectangle(SDL_Rect *rect);
};

class HexagonGrid : public Grid
{
public:
    HexagonGrid(Sint16 grid_radius, Layout *layout)
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
                FieldMeta *meta = new FieldMeta(new_field, this->default_player);
                this->fields.insert({new_field, meta});
            }
        }
    }

    FieldMeta *get_neighbor(FieldMeta *field, Uint8 direction);

    Cluster *get_cluster(FieldMeta *field, Cluster *visited = nullptr);

    bool render(SDL_Renderer *renderer);

    Sint16 get_radius() { return radius * layout->size; }

private:
    Sint16 radius;
};


#endif

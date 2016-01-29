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
#include <set>
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
#include "Wrapper.hpp"

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

    inline bool operator==(const Field &rhs) const
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

    std::vector<SDL_Point> field_to_polygon_sdl(const Layout *layout) const;

    static Field cubic_round(double x, double y, double z);

    static Field hex_direction(Uint8 direction);
};

// from upper right corner
const std::vector<Field> hex_directions = {{+1, -1, 0},
                                           {+1, 0,  -1},
                                           {0,  +1, -1},
                                           {-1, +1, 0},
                                           {-1, 0,  +1},
                                           {0,  -1, +1}};

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

struct Resource
{
    Uint32 circle;
    Uint32 triangle;
    Uint32 square;

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
    Regeneration_1,
    Regeneration_2,
    Regeneration_3,
    Reproduction_1,
    Reproduction_2,
    Reproduction_3
};

const std::vector<Upgrade> UPGRADES = {Regeneration_1, Regeneration_2, Regeneration_3, Reproduction_1, Reproduction_2,
                                       Reproduction_3};

const int NUM_UPGRADES = 6;

typedef std::bitset<NUM_UPGRADES> UpgradeFlags;

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

const std::unordered_map<Upgrade, std::string> UPGRADE_NAMES(
        {
                {Regeneration_1, "Regeneration 1"},
                {Regeneration_2, "Regeneration 2"},
                {Regeneration_3, "Regeneration 3"},
                {Reproduction_1, "Reproduction 1"},
                {Reproduction_2, "Reproduction 2"},
                {Reproduction_3, "Reproduction 3"}
        }
);

const std::unordered_map<Upgrade, std::string> UPGRADE_TEXTS(
        {
                {Regeneration_1, "Resources yield 2x their base resources per turn."},
                {Regeneration_2, "Resources yield 4x their base resources per turn."},
                {Regeneration_3, "Resources yield 8x their base resources per turn."},
                {Reproduction_1, "Increase the chance for expanding to a new field at the end of the turn by 25%."},
                {Reproduction_2, "Increase the chance for expanding to a new field at the end of the turn by 50%."},
                {Reproduction_3, "Increase the chance for expanding to a new field at the end of the turn by 75%."}
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
        int number = (this->uuid.data[0] << 8) | this->uuid.data[1];
        descriptor << this->name << " (" << std::hex << number << ")";
        return descriptor.str();
    }

    bool fight(FieldMeta *field);

    boost::uuids::uuid get_id() { return this->uuid; }

    inline bool operator==(const Player &rhs) const
    {
        return this->uuid == rhs.uuid;
    }

    inline bool operator!=(const Player &rhs) const
    {
        return !(*this == rhs);
    }

    void handle_event(SDL_Event *event);

    static Player *current_player;

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
    FieldMeta(HexagonGrid *grid_, Field field_, Player *owner_)
            : grid(grid_), field(field_), owner(owner_), changed(true)
    {
        std::default_random_engine generator;
        std::normal_distribution<double> distribution(0.0, 1.0);
        this->reproduction = distribution(generator);
        this->upgrades = 0;
        static std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<Uint8> distro(0, 1);
        this->resources_base.circle = distro(rng);
        this->resources_base.triangle = distro(rng);
        this->resources_base.square = distro(rng);
        this->offense = 0;
        this->defense = 0;
        this->regenerate_resources();
    }

    HexagonGrid *get_grid() { return this->grid; }

    int get_offense() { return this->offense; }

    int get_defense() { return this->defense; }

    void set_offense(int off) { this->offense = off; }

    void set_defense(int def) { this->defense = def; }

    Field get_field() { return this->field; }

    Player *get_owner() { return this->owner; }

    void set_owner(Player *player) { this->owner = player; }

    void load(SDL_Renderer *renderer, Layout *layout);

    Resource get_resources() { return this->resources; }

    UpgradeFlags get_upgrades() { return this->upgrades; }

    void consume_resources(Resource costs);

    void regenerate_resources();

    bool upgrade(Upgrade upgrade);

    void handle_event(const SDL_Event *event);

    FieldMeta *get_neighbor(Uint8 direction);

    double get_reproduction() { return this->reproduction; }

private:
    double reproduction;
    bool changed;
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

class HexagonGrid
{
public:
    HexagonGrid(Sint16 grid_radius, Layout *layout_, Renderer *renderer_)
            : layout(layout_), radius(grid_radius), renderer(renderer_)
    {
        this->attack_marker = nullptr;
        this->texture = nullptr;
        this->panning = false;
        std::unordered_map<Field, FieldMeta *> fields = std::unordered_map<Field, FieldMeta *>();
        this->default_player = new Player();
        // first lower half, then upper half
        Field new_field = {0, 0, 0};
        for (Sint16 x = -grid_radius; x <= grid_radius; x++)
        {
            Sint16 y_l = (-grid_radius > -x - grid_radius) ? -grid_radius : -x - grid_radius;
            Sint16 y_u = (grid_radius < -x + grid_radius) ? grid_radius : -x + grid_radius;
            for (Sint16 y = y_l; y <= y_u; y++)
            {
                Sint16 z = -x - y;
                new_field = {x, y, z};
                FieldMeta *meta = new FieldMeta(this, new_field, this->default_player);
                this->fields.insert({new_field, meta});
            }
        }
        this->marker = new FieldMeta(this, new_field, this->default_player);
        this->load();
    }

    ~HexagonGrid()
    {
        for (auto const &elem : this->fields)
        {
            delete elem.second;
        }
        delete this->default_player;
        SDL_DestroyTexture(this->texture);
    }

    FieldMeta *get_neighbor(FieldMeta *field, Uint8 direction);

    Cluster get_cluster(FieldMeta *field);

    void render(Renderer *renderer);

    void load();

    Sint16 get_radius() { return radius * layout->size; }

    void move(SDL_Point move);

    void update_marker();

    void update_dimensions(SDL_Point dimensions);

    Resource get_resources_of_cluster(Cluster *cluster);

    Resource consume_resources_of_cluster(Cluster *cluster, Resource costs);

    FieldMeta *point_to_field(const Point p);

    Point field_to_point(FieldMeta *field);

    FieldMeta *get_field(Field field);

    void handle_event(SDL_Event *event);

    bool place(Player *player, FieldMeta *center);

    void set_selecting(bool state) { this->placing = state; }

    FieldMeta *get_attack_marker() { return this->attack_marker; }

private:
    bool changed;
    bool placing;
    FieldMeta *attack_marker;
    Renderer *renderer;
    SDL_Texture *texture;
    std::unordered_map<Field, FieldMeta *> fields;
    Layout *layout;
    FieldMeta *marker;
    bool panning;
    Player *default_player;
    Sint16 radius;

    bool on_rectangle(SDL_Rect *rect);
};

bool inside_target(const SDL_Rect *target, const SDL_Point *position);

#endif

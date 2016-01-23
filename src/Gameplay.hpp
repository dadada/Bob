#include <random>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <bitset>
#include "Grid.hpp"
#include <stdexcept>

#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

#define NUM_UPGRADES 10

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

class Tagged
{
private:
    boost::uuids::uuid tag;

    int state;
public:
    // construct new
    Tagged()
            : tag(boost::uuids::random_generator()()), state(0) { }

    // construct with state
    explicit Tagged(int state)
            : tag(boost::uuids::random_generator()()), state(state) { }

    // clone
    Tagged(Tagged const &rhs)
            : tag(rhs.tag), state(rhs.state) { }

    bool operator==(Tagged const &rhs) const
    {
        return tag == rhs.tag;
    }

    bool operator!=(Tagged const &rhs) const
    {
        return !(tag == rhs.tag);
    }

    int get_state() const { return state; }

    void set_state(int new_state) { state = new_state; }

    boost::uuids::uuid get_tag() { return tag; }

    void set_tag(boost::uuids::uuid uuid_) { tag = uuid_; }
};

class Player;

class FieldMeta;

class Player : public Tagged
{

public:
    Player(std::string name_)
            : name(name_), Tagged()
    {
        // use the last 24 bits of the tag for the color
        boost::uuids::uuid id = get_tag();
        uint8_t *data = id.data;
        this->color = {data[13], data[14], data[15], 0xff};
    }

    SDL_Color get_color() { return color; }

    std::string get_name()
    {
        std::ostringstream descriptor;
        boost::uuids::uuid id = get_tag();
        uint8_t *data = id.data;
        Uint16 number = (data[14] << 8) | (data[15]);
        descriptor << this->name << " (" << std::hex << number << ")";
        return descriptor.str();
    }

    bool fight(FieldMeta *field);

private:
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

    void handle_event(const SDL_Event *event);

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
        this->default_player = new Player("Default");
        this->marker = new FieldMeta(f, default_player);
    };

    ~Grid()
    {
        for (auto const &elem : this->fields)
        {
            delete elem.second;
        }
    }

    Player *get_default_player() { return this->default_player; }

    void move(SDL_Point move);

    void update_marker();

    virtual bool render(SDL_Renderer *renderer) { return false; };

    void handle_event(SDL_Event *event);

    void update_box(SDL_Point dimensions);

    Resource get_resources_of_cluster(const Cluster *cluster);

    virtual FieldMeta *get_neighbor(FieldMeta *field, Uint8 direction) = 0;

    virtual Cluster *get_cluster(FieldMeta *field, Cluster *visited = nullptr) = 0;

    Resource consume_resources_of_cluster(Cluster *cluster, Resource costs);

    FieldMeta *point_to_field(const Point p);

protected:
    Player *default_player;
    std::unordered_map<Field, FieldMeta *> fields;
    Layout *layout;
    FieldMeta *marker;
    bool panning;

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
                FieldMeta *meta = new FieldMeta(new_field, default_player);
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

#include <random>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include "Grid.hpp"


#ifndef Upgrade
typedef enum
{
    UPGRADE_FIRST_UPGRADE = 0,
    UPGRADE_REGENERATION_1,
    UPGRADE_REGENERATION_2,
    UPGRADE_REGENERATION_3,
    UPGRADE_REPRODUCTION
} Upgrade;
#endif

#ifndef Tagged

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

#endif

#ifndef Player

class Player : public Tagged
{
private:
    SDL_Color color;
    std::string name;

public:
    Player(std::string name_)
            : name(name_), Tagged()
    {
        // use the last 24 bits of the tag for the color
        boost::uuids::uuid id = get_tag();
        uint8_t *data = id.data;
        this->color = {data[13], data[14], data[15], 0xff};
    }

    Player(std::string name_, boost::uuids::uuid tag_)
            : Tagged(), name(name_)
    {
        this->set_tag(tag_);
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

    bool fight(Field field);

    static Player default_player;
};

#endif

#ifndef Resource

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

#endif


#ifndef FieldMeta

class FieldMeta
{
public:
    FieldMeta(Field field_, Player *owner_ = &(Player::default_player))
            : field(field_), owner(owner_)
    {
        this->upgrades = 0;
        static std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<Uint8> distro(0, 1);
        this->resources_base.circle = distro(rng);
        this->resources_base.triangle = distro(rng);
        this->resources_base.square = distro(rng);
        std::pair<Field, FieldMeta *> pair(field, this);
        this->fields.insert(pair);
        this->offense = 1;
        this->defense = 1;
    }

    int get_offense();

    int get_defense();

    static FieldMeta *get_meta(Field field);

    Field get_field();

    Player *get_owner();

    void set_owner(Player *player);

    //void set_owner(Player *player);

    void render(SDL_Renderer *renderer, Layout *layout);

    Resource get_resources();

    Resource get_resources_of_cluster();

    Resource get_resources_of_cluster(std::unordered_set<FieldMeta *> cluster);

    std::unordered_set<FieldMeta *> get_cluster(
            std::unordered_set<FieldMeta *> visited = std::unordered_set<FieldMeta *>());

    Uint32 get_upgrades();

    void regenerate_resources();

    bool upgrade(Upgrade upgrade);

    Resource consume_resources_of_cluster(Resource costs);

private:
    const Field field;
    static std::unordered_map<Field, FieldMeta *> fields;
    Player *owner;
    Uint32 upgrades;
    Resource resources_base; // without upgrades applied, used as basis of regeneration
    Resource resources; // actual current resources
    int offense;
    int defense;
};

#endif

#ifndef Cluster
typedef std::unordered_set<FieldMeta *> Cluster;
#endif


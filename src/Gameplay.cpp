#include "Gameplay.hpp"

Player Player::default_player = Player("Default");

std::unordered_map<Field, FieldMeta *> FieldMeta::fields = std::unordered_map<Field, FieldMeta *>();

Player *FieldMeta::get_owner()
{
    return this->owner;
}

void FieldMeta::set_owner(Player *player)
{
    this->owner = player;
}

Field FieldMeta::get_field() { return this->field; }

void FieldMeta::render(SDL_Renderer *renderer, Layout *layout)
{
    Point precise_location = field_to_point(this->field, layout);
    SDL_Point location;
    location.x = (int) precise_location.x;
    location.y = (int) precise_location.y;
    std::vector<Point> polygon = field_to_polygon(this->field, layout);
    SDL_Color color = this->owner->get_color();
    Sint16 vx[6];
    Sint16 vy[6];
    for (int i = 0; i < 6; i++)
    {
        vx[i] = (Sint16) polygon[i].x;
        vy[i] = (Sint16) polygon[i].y;
    }
    filledPolygonRGBA(renderer, vx, vy, 6, color.r, color.g, color.b, 0x22);
    SDL_Color inverse;
    inverse.r = (Uint8) (0xff - color.r);
    inverse.g = (Uint8) (0xff - color.g);
    inverse.b = (Uint8) (0xff - color.b);
    inverse.a = 0xff;

    Uint16 resource_size = (Uint16) (layout->size / 3);
    if (this->resources_base.circle > 0)
    {
        circleRGBA(renderer, (Sint16) (location.x), (Sint16) (location.y), (Sint16) (resource_size), inverse.r,
                   inverse.g, inverse.b, inverse.a);
    }
    if (this->resources_base.triangle > 0)
    {
        static const SDL_Point trigon[] = {{-1, 1},
                                           {1,  1},
                                           {0,  -1}};
        for (int i = 0; i < 3; i++)
        {
            vx[i] = (Sint16) (location.x + (trigon[i].x) * resource_size);
            vy[i] = (Sint16) (location.y + (trigon[i].y) * resource_size);
        }
        trigonRGBA(renderer, vx[0], vy[0], vx[1], vy[1], vx[2], vy[2], inverse.r, inverse.g, inverse.b, inverse.a);
    }

    if (this->resources_base.square > 0)
    {

        static const SDL_Point square[] = {{-1, -1},
                                           {-1, 1},
                                           {1,  1},
                                           {1,  -1}};
        for (int i = 0; i < 4; i++)
        {
            vx[i] = (Sint16) (location.x + (square[i].x) * resource_size);
            vy[i] = (Sint16) (location.y + (square[i].y) * resource_size);
        }
        polygonRGBA(renderer, vx, vy, 4, inverse.r, inverse.g, inverse.b, inverse.a);
    }
}


void FieldMeta::regenerate_resources()
{
    resources = resources_base;
    switch (this->upgrades)
    {
        case (UPGRADE_REGENERATION_1):
            resources *= 2;
        case (UPGRADE_REGENERATION_2):
            resources *= 2;
        case (UPGRADE_REGENERATION_3):
            resources *= 2;
        default:
            break;
    }
}

Resource FieldMeta::get_resources()
{
    return this->resources;
}

Resource FieldMeta::get_resources_of_cluster()
{
    Resource res = {0, 0, 0};
    std::unordered_set<FieldMeta *> *cluster = new std::unordered_set<FieldMeta *>();
    this->get_cluster(cluster);
    for (FieldMeta *elem : *cluster)
    {
        Resource r_plus = elem->get_resources();
        res += r_plus;
    }
    delete cluster;
    return res;
}

Uint32 FieldMeta::get_upgrades() { return this->upgrades; }

bool FieldMeta::upgrade(Upgrade upgrade)
{
    // check available resources for cluster and consume resources
    Resource cluster_resources = this->get_resources_of_cluster();
    Resource costs;
    switch (upgrade)
    {
        case UPGRADE_REGENERATION_1:
            costs = {(1 << 1), (1 << 1), (1 << 1)};
            break;
        case UPGRADE_REGENERATION_2:
            costs = {(1 << 2), (1 << 2), (1 << 2)};
            break;
        case UPGRADE_REGENERATION_3:
            costs = {(1 << 3), (1 << 3), (1 << 3)};

            break;
        default:
            std::cout << "Unknown update: " << upgrade;
            break;
    }
    if (cluster_resources > costs)
    {
        this->resources -= costs;
        this->upgrades |= upgrade;
    }
    return false;
}

std::unordered_set<FieldMeta *> *FieldMeta::get_cluster(std::unordered_set<FieldMeta *> *visited)
{
    assert(visited != nullptr);

    if (visited->find(this) != visited->end()) // already been here before
        return visited;

    visited->insert(this);

    for (Uint8 i = 0; i < 6; i++)
    {
        auto neighbor_pair = FieldMeta::fields.find(hex_neighbor(i, this->get_field()));
        FieldMeta *neighbor = neighbor_pair->second;
        if (neighbor->get_owner() != this->get_owner()) // ignore meta if field not owned by specified owner
            return visited;
        else
            return neighbor->get_cluster(visited);
    }
    return visited;
}

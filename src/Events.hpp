#include <SDL2/SDL.h>
#include <boost/uuid/uuid.hpp>
#include "Grid.hpp"
#include "Gameplay.hpp"

#ifndef _EVENTS_H
#define _EVENTS_H

class Timer
{
private:
    Uint32 timer_started;
public:
    void start_timer();

    Uint32 get_timer();

    Uint32 reset_timer();
};

struct NextTurn
{
    boost::uuids::uuid player;
    Uint32 time_elapsed;
};

struct MarkerUpdate
{
    FieldMeta *field;
};

struct FieldUpdate
{
    FieldMeta *field;
};

Uint32 register_events(Uint32 n);

#endif
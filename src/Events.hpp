#include <SDL2/SDL.h>
#include <boost/uuid/uuid.hpp>
#include "Grid.hpp"
#include "Gameplay.hpp"

#ifndef _EVENTS_H
#define _EVENTS_H

class EventContext
{
public:
    enum Bob_Event
    {
        Bob_NextTurnEvent,
        Bob_FieldUpdateEvent
    };

    EventContext(Uint32 num_events)
            : base_event(register_events(num_events)) { }

    Uint32 get_event(Bob_Event event);

private:
    const Uint32 base_event;

    static Uint32 register_events(Uint32 n);
};

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

struct FieldUpdate
{
    FieldMeta *field;
};

#endif
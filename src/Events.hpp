#ifndef _EVENTS_H
#define _EVENTS_H

#include <SDL2/SDL.h>
#include <boost/uuid/uuid.hpp>

enum Bob_Event
{
    BOB_NEXTTURNEVENT,
    BOB_FIELDUPDATEEVENT
};

class EventContext
{
public:
    EventContext()
            : base_event(register_events(num_events)) { }

    const Uint32 base_event;

private:

    static const int num_events = 2;

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
    Sint16 x;
    Sint16 y;
    Sint16 z;
};

#endif
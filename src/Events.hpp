#ifndef _EVENTS_H
#define _EVENTS_H

#include <SDL2/SDL.h>
#include <boost/uuid/uuid.hpp>
#include "Exceptions.hpp"

#ifndef _EVENT_TYPES
#define _EVENT_TYPES
extern const Uint32 BOB_NEXTROUNDEVENT;
extern const Uint32 BOB_MARKERUPDATE;
extern const Uint32 BOB_FIELDUPDATEEVENT;
extern const Uint32 BOB_FIELDSELECTEDEVENT;
extern const Uint32 BOB_FIELDUPGRADEVENT;
extern const Uint32 BOB_NEXTTURNEVENT;
#endif

Uint32 register_events(Uint32 n);

class Timer
{
private:
    Uint32 timer_started;
public:
    void start_timer();

    Uint32 get_timer();

    Uint32 reset_timer();

    static bool MOUSE_LOCKED;
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

void trigger_event(Uint32 type, Sint32 code, void *data1, void *data2);

#endif
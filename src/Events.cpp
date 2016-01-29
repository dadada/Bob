#include "Events.hpp"

const Uint32 BOB_NEXTROUNDEVENT = register_events(1);
const Uint32 BOB_MARKERUPDATE = register_events(1);
const Uint32 BOB_FIELDUPDATEEVENT = register_events(1);
const Uint32 BOB_FIELDSELECTEDEVENT = register_events(1);
const Uint32 BOB_FIELDUPGRADEVENT = register_events(1);
const Uint32 BOB_NEXTTURNEVENT = register_events(1);
const Uint32 BOB_ATTACKEVENT = register_events(1);
const Uint32 BOB_PLAYERADDED = register_events(1);

bool Timer::MOUSE_LOCKED = false;

void Timer::start_timer()
{
    this->timer_started = SDL_GetTicks();
}

Uint32 Timer::get_timer()
{
    Uint32 ticks_passed = SDL_GetTicks() - this->timer_started;
    return ticks_passed;
}

Uint32 Timer::reset_timer()
{
    Uint32 ticks_passed = this->get_timer();
    this->timer_started = SDL_GetTicks();
    return ticks_passed;
}

Uint32 register_events(Uint32 n)
{
    Uint32 base_event = SDL_RegisterEvents(n);
    if (base_event == ((Uint32) - 1))
        throw SDL_Exception("Failed to register events!");
    return base_event;
}

void trigger_event(Uint32 type, Sint32 code, void *data1, void *data2)
{
    SDL_Event event;
    SDL_memset(&event, 0, sizeof(event)); /* or SDL_zero(event) */
    event.type = type;
    event.user.code = code;
    event.user.data1 = static_cast<void *>(data1);
    event.user.data2 = data2;
    SDL_PushEvent(&event);
}
#include "Events.hpp"

const Uint32 BOB_NEXTTURNEVENT = register_events(1);
const Uint32 BOB_MARKERUPDATE = register_events(1);
const Uint32 BOB_FIELDUPDATEEVENT = register_events(1);
const Uint32 BOB_FIELDSELECTED = register_events(1);
const Uint32 BOB_FIELDUPGRADEVENT = register_events(1);


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
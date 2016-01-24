#include "Events.hpp"

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

Uint32 EventContext::register_events(Uint32 num_events)
{
    return SDL_RegisterEvents(num_events);
}

Uint32 EventContext::get_event(Bob_Event event)
{
    return this->base_event + event;
}
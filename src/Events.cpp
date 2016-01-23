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

Uint32 register_events(Uint32 n)
{
    return SDL_RegisterEvents(n);
}
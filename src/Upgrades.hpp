#include <SDL2/SDL.h>

#ifndef Upgrade

class Upgrade
{
    SDL_Color color;
public:
    static Upgrade *Instance();

private:
    Upgrade()
    { };

    Upgrade(Upgrade const &)
    { };

    Upgrade &operator=(Upgrade const &)
    { };
    static Upgrade *m_pInstance;
};

#endif
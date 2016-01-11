#include <iostream>
#include <utility>
#include <SDL2/SDL.h>
#include "GameMap.hpp"

#ifndef DATA_PATH
#endif
#ifndef SCREEN_WIDTH
const int SCREEN_WIDTH = 800;
#endif
#ifndef SCREEN_HEIGHT
const int SCREEN_HEIGHT = 800;
#endif

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
Uint32 rmask = 0xff000000;
Uint32 gmask = 0x00ff0000;
Uint32 bmask = 0x0000ff00;
Uint32 amask = 0x000000ff;
#else
Uint32 rmask = 0x000000ff;
Uint32 gmask = 0x0000ff00;
Uint32 bmask = 0x00ff0000;
Uint32 amask = 0xff000000;
#endif



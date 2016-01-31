//
// Created by tim on 25.01.16.
//

#ifndef BOB_PIXELMASK_H
#define BOB_PIXELMASK_H

#include <SDL2/SDL_pixels.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define rmask 0xff000000
#define gmask 0x00ff0000
#define bmask 0x0000ff00
#define amask 0x000000ff
#else
#define rmask 0x000000ff
#define gmask 0x0000ff00
#define bmask 0x00ff0000
#define amask 0xff000000
#endif

#endif //BOB_PIXELMASK_H

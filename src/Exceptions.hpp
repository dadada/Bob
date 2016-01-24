#include <stdexcept>
#include <SDL2/SDL_ttf.h>

#ifndef BOB_EXCEPTIONS_H
#define BOB_EXCEPTIONS_H

class SDL_Exception : public std::runtime_error
{
public:
    SDL_Exception(const std::string &what_arg)
            : std::runtime_error(what_arg) { }

    const char *what() const noexcept
    {
        return SDL_GetError();
    }
};

class SDL_WindowException : public SDL_Exception
{
public:
    SDL_WindowException()
            : SDL_Exception("SDL_WindowException") { }
};

class SDL_RendererException : public SDL_Exception
{
public:
    SDL_RendererException()
            : SDL_Exception("SDL_RendererException") { }
};

class SDL_TextureException : public SDL_Exception
{
public:
    SDL_TextureException()
            : SDL_Exception("SDL_TextureException") { }
};

class SDL_TTFException : public SDL_Exception
{
public:
    SDL_TTFException()
            : SDL_Exception("SDL_TTFException") { }
};

#endif //BOB_EXCEPTIONS_H

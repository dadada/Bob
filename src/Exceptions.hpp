#include <stdexcept>

#ifndef BOB_EXCEPTIONS_H
#define BOB_EXCEPTIONS_H

class SDL_Exception : public std::exception
{
public:
    virtual const char *what() const throw()
    {
        return "SDL_Exception";
    }
};

class SDL_WindowException : public SDL_Exception
{
public:
    virtual const char *what() const throw()
    {
        return "SDL_WindowException";
    }
};

class SDL_RendererException : public SDL_Exception
{
public:
    virtual const char *what() const throw()
    {
        return "SDL_RendererException";
    }
};

class SDL_TextureException : public SDL_Exception
{
public:
    virtual const char *what() const throw()
    {
        return "SDL_TextureException";
    }
};

#endif //BOB_EXCEPTIONS_H

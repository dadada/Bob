#include <string>
#include "Main.hpp"

void logSDLError(std::ostream &os, const std::string &msg) {
    os << msg << " error:" << SDL_GetError() << std::endl;
}

SDL_Window *init_window() {
    SDL_Window *window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (window == NULL) {
        logSDLError(std::cout, "SDL_CreateWindow");
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    return window;
}

SDL_Renderer *init_renderer(SDL_Window *window) {
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        logSDLError(std::cout, "SDL_CreateRenderer");

    }
    return renderer;
}

SDL_Surface *init_surface(int width, int height) {

    //SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
    SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        logSDLError(std::cout, "SDL_CreateSurface");
    }
    return surface;
}

SDL_Texture *init_texture(SDL_Renderer *renderer, SDL_Surface *surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        logSDLError(std::cout, "SDL_CreateTexture");
    }
    return texture;
}

int main(int, char **) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        logSDLError(std::cout, "SDL_init");
        return 1;
    }
    SDL_Window *window = init_window();
    if (!window) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *renderer = init_renderer(window);
    if (!renderer) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface *surface = init_surface(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!surface) {
        SDL_FreeSurface(surface);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture *texture = init_texture(renderer, surface);
    if (!texture) {
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    const SDL_Point center = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    const Layout standard = Layout(pointy_orientation, 20.0, center);
    const SDL_Color color = {0, 255, 255, 255};
    HexagonGrid *grid = new HexagonGrid(4);
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                quit = true;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        grid->render(renderer, color, &standard);
        SDL_RenderPresent(renderer);
    }
    delete grid;
    return 0;
}

// #ifdef _WIN32
// #define SDL_MAIN_HANDLED
// #endif
#include <SDL.h>
#include "core.h"

structdef(UIState) {
    i32 mousex;
    i32 mousey;
    i32 mousedown;
    i32 hotitem;
    i32 activeitem;
};
UIState uistate;
u8 r, g, b;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        SDL_Log("SDL_Init failed (%s)", SDL_GetError());
        return 1;
    }
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer)) {
        SDL_Log("SDL_CreateWindowAndRenderer failed (%s)", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Rect rect = {
        .x = 0,
        .y = 0,
        .w = 32,
        .h = 32,
    };
    r = g = 255;
    b = 0;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEMOTION:
                    uistate.mousex = event.motion.x;
                    uistate.mousey = event.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button) {
                        uistate.mousedown = true;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button) {
                        uistate.mousedown = false;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_ESCAPE:
                            running = false;
                            break;
                        case SDL_SCANCODE_A:
                            printf("A\n");
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }
        printf("mouse: {%d, %d}\n", uistate.mousex, uistate.mousey);
        rect.x = uistate.mousex;
        rect.y = uistate.mousey;
        g = 0xFF << (uistate.mousedown * 8);
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

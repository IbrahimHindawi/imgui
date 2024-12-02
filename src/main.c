#define UNICODE
#define WIN32_LEAN_AND_MEAN
// #define _CONSOLE

#include <windows.h>
#include <SDL3/SDL.h>
#include "core.h"

structdef(UIState) {
    i32 mousex;
    i32 mousey;
    i32 mousedown;
    i32 hotitem;
    i32 activeitem;
};

UIState uistate;

#ifdef _CONSOLE
int main(int argc, char* argv[]) {
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nShowCmd) {
#endif

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed (%s)", SDL_GetError());
        return 1;
    }
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    printf("init\n");

    if (!SDL_CreateWindowAndRenderer("SDL3", 640, 480, 0, &window, &renderer)) {
        SDL_Log("SDL_CreateWindowAndRenderer failed (%s)", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_FRect r = {
        .x = 0,
        .y = 0,
        .w = 128,
        .h = 128,
    };

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.scancode) {
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
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &r);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

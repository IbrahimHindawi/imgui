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
SDL_Rect rect;

bool regionhit(i32 x, i32 y, i32 w, i32 h) {
  if (uistate.mousex < x ||
      uistate.mousey < y ||
      uistate.mousex >= x + w ||
      uistate.mousey >= y + h) {
    return false;
  }
  return true;
}

void imgui_prepare() {
  uistate.hotitem = 0;
}

// Finish up after IMGUI code
void imgui_finish() {
    if (uistate.mousedown == 0) {
        uistate.activeitem = 0;
    } else {
        if (uistate.activeitem == 0) {
            uistate.activeitem = -1;
        }
    }
}

bool button(SDL_Renderer *renderer, int id, int x, int y) {
    // Check whether the button should be hot
    SDL_Rect rect = { .x = x, .y = y, .w = 64, .h = 48, };
    if (regionhit(x, y, rect.w, rect.h)) {
        uistate.hotitem = id;
        if (uistate.activeitem == 0 && uistate.mousedown) {
            uistate.activeitem = id;
        }
    }

    // Render button 
    // x, y, w, h, color
    // drawrect(x + 8, y + 8, 64, 48, 0);
    rect.x += 8; rect.y += 8;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rect);
    rect.x -= 8; rect.y -= 8;
    if (uistate.hotitem == id) {
        if (uistate.activeitem == id) {
            // Button is both 'hot' and 'active'
            // SDL_RenderFillRect(x + 2, y + 2, 64, 48, 0xffffff);
            rect.x += 2; rect.y += 2;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &rect);
            rect.x -= 2; rect.y -= 2;
        } else {
            // Button is merely 'hot'
            // SDL_RenderFillRect(x, y, 64, 48, 0xffffff);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &rect);
        }
    } else {
        // // button is not hot, but it may be active    
        // SDL_RenderFillRect(x, y, 64, 48, 0xaaaaaa);
        SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
    }
    if (uistate.mousedown == 0 && uistate.hotitem == id && uistate.activeitem == id) {
        return true;
    }

    // Otherwise, no clicky.
    return false;
}

i32 main(i32 argc, char *argv[]) {
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
    rect = (SDL_Rect) { .w = 32, .h = 32, };
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
        // clear screen
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        imgui_prepare();

        button(renderer, 2, 32, 32);

        printf("mouse: {%d, %d}\n", uistate.mousex, uistate.mousey);
        rect.x = uistate.mousex;
        rect.y = uistate.mousey;
        g = 0xFF << (uistate.mousedown * 8);

        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);

        imgui_finish();

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

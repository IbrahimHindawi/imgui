// #ifdef _WIN32
// #define SDL_MAIN_HANDLED
// #endif
#include <SDL.h>
#include "core.h"

// If you're going to render widgets to the same
// UI from different source files, you can avoid
// ID collisions by defining IMGUI_SRC_ID before
// this define block:
#ifdef IMGUI_SRC_ID
#define GEN_ID ((IMGUI_SRC_ID) + (__LINE__))
#else
#define GEN_ID (__LINE__)
#endif

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

// Simple scroll bar IMGUI widget
bool slider(SDL_Renderer *renderer, int id, int x, int y, int max, int *value) {
    SDL_Rect rect = { .x = x, .y = y, .w = 32, .h = 256, };
    // Calculate mouse cursor's relative y offset
    int ypos = ((256 - 16) * *value) / max;

    // Check for hotness
    if (regionhit(x+8, y+8, 32, 256)) {
        uistate.hotitem = id;
        if (uistate.activeitem == 0 && uistate.mousedown) {
            uistate.activeitem = id;
        }
    }
    // Render the scrollbar
    // drawrect(x, y, 32, 256+16, 0x777777);
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rect);

    if (uistate.activeitem == id || uistate.hotitem == id) {
        // drawrect(x+8, y+8 + ypos, 16, 16, 0xffffff);
        rect.x += 8; rect.y += 8 + ypos;
        rect.w = rect.h = 16;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
        rect.x -= 8; rect.y -= 8 + ypos;
    } else {
        // drawrect(x+8, y+8 + ypos, 16, 16, 0xaaaaaa);
        rect.x += 8; rect.y += 8 + ypos;
        rect.w = rect.h = 16;
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
        rect.x -= 8; rect.y -= 8 + ypos;
    }
    // Update widget value
    if (uistate.activeitem == id) {
        int mousepos = uistate.mousey - (y + 8);
        if (mousepos < 0) {
            mousepos = 0;
        }
        if (mousepos > 255) {
            mousepos = 255;
        }
        int v = (mousepos * max) / 255;
        if (v != *value) {
          *value = v;
          return true;
        }
    }
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
    r = g = b = 128;

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
        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        imgui_prepare();

        button(renderer, GEN_ID, 32 + 128 * 0, 32);
        button(renderer, GEN_ID, 32 + 128 * 1, 32);
        if (button(renderer, GEN_ID, 32 + 128 * 2, 32)) {
            r += 28;
            g += 12;
            b += 2;
        }
        static int bgcolor = 13;
        int slidervalue = bgcolor & 0xff;
        if (slider(renderer, GEN_ID, 500, 40, 255, &slidervalue)) {
            bgcolor = (bgcolor & 0xffff00) | slidervalue;
        }

        // printf("mouse: {%d, %d}\n", uistate.mousex, uistate.mousey);
        rect.x = uistate.mousex; rect.y = uistate.mousey;

        SDL_SetRenderDrawColor(renderer, 255, 0xFF << (uistate.mousedown * 8), 255, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);

        imgui_finish();

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}

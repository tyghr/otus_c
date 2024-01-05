#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
// #include <stdint.h>
// #include <stdarg.h>
// #include <string.h>
// #include <math.h>
// #include <assert.h>
// #include <limits.h>

#include <SDL2/SDL.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_renderer.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define MENU_SKIP 0.1

long getCurrentTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

int main() {
    /* Platform */
    SDL_Window *window;
    SDL_Renderer *renderer;
    int flags = 0;
    float font_scale = 1;

    /* GUI */
    struct nk_context *ctx;
    struct nk_colorf bg;

    /* SDL setup */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Escape from Agile",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL) {
        SDL_Log("Error SDL_CreateWindow %s", SDL_GetError());
        exit(-1);
    }

    flags |= SDL_RENDERER_ACCELERATED;
    flags |= SDL_RENDERER_PRESENTVSYNC;

    renderer = SDL_CreateRenderer(window, -1, flags);

    if (renderer == NULL) {
        SDL_Log("Error SDL_CreateRenderer %s", SDL_GetError());
        exit(-1);
    }

    /* scale the renderer output for High-DPI displays */
    {
        int render_w, render_h;
        int window_w, window_h;
        float scale_x, scale_y;
        SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
        SDL_GetWindowSize(window, &window_w, &window_h);
        scale_x = (float)(render_w) / (float)(window_w);
        scale_y = (float)(render_h) / (float)(window_h);
        SDL_RenderSetScale(renderer, scale_x, scale_y);
        font_scale = scale_y;
    }

    /* GUI */
    ctx = nk_sdl_init(window, renderer);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {
        struct nk_font_atlas *atlas;
        struct nk_font_config config = nk_font_config(0);
        struct nk_font *font;

        /* set up the font atlas and add desired font; note that font sizes are multiplied by font_scale to produce better results at higher DPIs */
        nk_sdl_font_stash_begin(&atlas);
        font = nk_font_atlas_add_default(atlas, 14 * font_scale, &config);
        nk_sdl_font_stash_end();

        /* this hack makes the font appear to be scaled down to the desired size and is only necessary when font_scale > 1 */
        font->handle.height /= font_scale;
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        nk_style_set_font(ctx, &font->handle);
    }

    bool menu_pause = true;

    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

    SDL_Surface *playerImage = SDL_LoadBMP("./player.bmp");
    SDL_Texture *playerTexture = SDL_CreateTextureFromSurface(renderer, playerImage);
    SDL_FreeSurface(playerImage);

    SDL_Surface *monsterImage = SDL_LoadBMP("./monster.bmp");
    SDL_Texture *monsterTexture = SDL_CreateTextureFromSurface(renderer, monsterImage);
    SDL_FreeSurface(monsterImage);

    int playerX = WINDOW_WIDTH / 2;
    int playerY = WINDOW_HEIGHT / 2;

    int monsterX = 1;
    int monsterY = 1;

    long prevMoveTime = getCurrentTime();
    long MS_PER_MOVE = 100000;

    static int monster_speed = 5;

    bool quit = false;
    while (!quit) {
        /* Input */
        SDL_Event event;

        nk_input_begin(ctx);
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                goto cleanup;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    menu_pause = !menu_pause;
                    break;
                case SDLK_LEFT:
                    playerX -= 10;
                    break;
                case SDLK_RIGHT:
                    playerX += 10;
                    break;
                case SDLK_UP:
                    playerY -= 10;
                    break;
                case SDLK_DOWN:
                    playerY += 10;
                    break;
                };
                break;
            }
            nk_sdl_handle_event(&event);
        }
        nk_input_end(ctx);

		if (!menu_pause) {
            /* calc character's movement */
            long currentTime = getCurrentTime();
            if ((currentTime - prevMoveTime) >= MS_PER_MOVE) {
                if (monsterX < playerX) {
                    monsterX += monster_speed;
                } else {
                    monsterX -= monster_speed;
                }
                if (monsterY < playerY) {
                    monsterY += monster_speed;
                } else {
                    monsterY -= monster_speed;
                }
                prevMoveTime = currentTime;
            }
        }

        SDL_Rect playerDstRect = {playerX, playerY, 64, 64};
        SDL_Rect monsterDstRect = {monsterX, monsterY, 64, 64};

		if (menu_pause) {
			/* GUI */

            if (nk_begin(ctx, "Menu pause", nk_rect(WINDOW_WIDTH * MENU_SKIP, WINDOW_HEIGHT * MENU_SKIP, WINDOW_WIDTH * (1 - 2 * MENU_SKIP), WINDOW_HEIGHT * (1 - 2 * MENU_SKIP)),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|
                NK_WINDOW_TITLE))
            {

                // nk_layout_row_dynamic(ctx, 20, 1);
				// nk_label(ctx, "background:", NK_TEXT_LEFT);
				// nk_layout_row_dynamic(ctx, 25, 1);
				// if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
				// 	nk_layout_row_dynamic(ctx, 120, 1);
				// 	bg = nk_color_picker(ctx, bg, NK_RGBA);
				// 	nk_layout_row_dynamic(ctx, 25, 1);
				// 	bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
				// 	bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
				// 	bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
				// 	bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
				// 	nk_combo_end(ctx);
				// }

                /* custom widget pixel width */
                nk_layout_row_begin(ctx, NK_DYNAMIC, 30, 3);
                {
                    nk_layout_row_push(ctx, 0.3);
                    nk_label(ctx, "Monster speed:", NK_TEXT_RIGHT);
                    nk_layout_row_push(ctx, 0.4);
                    nk_slider_int(ctx, 1, &monster_speed, 20, 1);
                }
                nk_layout_row_end(ctx);

                /* fixed widget pixel width */
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "New Game")) {
                    playerX = WINDOW_WIDTH / 2;
                    playerY = WINDOW_HEIGHT / 2;

                    monsterX = 1;
                    monsterY = 1;

                    menu_pause = false;
                }

                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Exit")) {
                    quit = true;
                }

            }
            nk_end(ctx);
        }

        SDL_SetRenderDrawColor(renderer, bg.r * 255, bg.g * 255, bg.b * 255, bg.a * 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, playerTexture, NULL, &playerDstRect);
        SDL_RenderCopy(renderer, monsterTexture, NULL, &monsterDstRect);

        nk_sdl_render(NK_ANTI_ALIASING_ON);

        SDL_RenderPresent(renderer);
    }

cleanup:
    nk_sdl_shutdown();
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(monsterTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

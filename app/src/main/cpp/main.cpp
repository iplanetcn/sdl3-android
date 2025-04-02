#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *texture;
    int done;
    SDL_Event event;

    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed (%s)", SDL_GetError());
        return 1;
    }

    if (!SDL_CreateWindowAndRenderer("Show a simple image", 0, 0, 0, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindowAndRenderer() failed: %s", SDL_GetError());
        return(2);
    }

    surface = IMG_Load("android_robot.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load texture: %s", SDL_GetError());
        return(2);
    }

    float targetSize = 200;
    float outlineSize = 240;
    int scw;
    int sch;
    SDL_GetWindowSizeInPixels(window, &scw, &sch);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Screen Size: width = %d, height = %d", scw, sch);

    SDL_FRect srcRect = {0, 0, float(texture->w), float(texture->h)};
    SDL_FRect targetRect = { (scw - targetSize)/2.0f, (sch - targetSize)/2.0f, targetSize, targetSize };

    SDL_FRect outlineRect = { (scw - outlineSize)/2.0f, (sch - outlineSize)/2.0f, outlineSize, outlineSize };

    done = 0;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                done = 1;
        }

        SDL_RenderTexture(renderer, texture, &srcRect, &targetRect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderRect(renderer, &outlineRect);
        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }
    SDL_DestroyTexture(texture);

    SDL_Quit();
    return(0);
}
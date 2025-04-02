/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL, standard IO, and, strings
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>
//#include <cstdio>
//#include <cstring>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed (%s)", SDL_GetError());
        return 1;
    }

    if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Hello World",
                                  "!! Your SDL project successfully runs on Android !!", NULL)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_ShowSimpleMessageBox failed (%s)", SDL_GetError());
        return 1;
    }

    SDL_Quit();
    return 0;
}




//Texture wrapper class
//#define toggle false

#if defined(toggle)
class LTexture {
public:
    //Initializes variables
    LTexture();

    //Deallocates memory
    ~LTexture();

    //Loads image at specified path
    bool loadFromFile(const std::string& path);

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
    //Creates image from font string
        bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
#endif

    //Creates blank texture
    bool createBlank(int width, int height, SDL_TextureAccess = SDL_TEXTUREACCESS_STREAMING);

    //Deallocates texture
    void free();

    //Set color modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    //Set blending
    void setBlendMode(SDL_BlendMode blending);

    //Set alpha modulation
    void setAlpha(Uint8 alpha);

    //Renders texture at given point
    void render(int x, int y, SDL_Rect *clip = nullptr, double angle = 0.0, SDL_Point *center = nullptr,
                SDL_FlipMode flip = SDL_FLIP_NONE);

    //Set self as render target
    void setAsRenderTarget();

    //Gets image dimensions
    int getWidth() const;

    int getHeight() const;

    //Pixel manipulators
    bool lockTexture();

    bool unlockTexture();

    void *getPixels();

    void copyPixels(void *pixels);

    int getPitch() const;

    Uint32 getPixel32(unsigned int x, unsigned int y);

private:
    //The actual hardware texture
    SDL_Texture *mTexture;
    void *mPixels;
    int mPitch;

    //Image dimensions
    int mWidth;
    int mHeight;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window *gWindow = nullptr;

//The window renderer
SDL_Renderer *gRenderer = nullptr;

//Screen dimensions
SDL_Rect gScreenRect = {0, 0, 320, 240};

//Scene textures
LTexture gSplashTexture;

LTexture::LTexture() {
    //Initialize
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
    mPixels = nullptr;
    mPitch = 0;
}

LTexture::~LTexture() {
    //Deallocate
    free();
}

bool LTexture::loadFromFile(const std::string& path) {
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture *newTexture = nullptr;

    //Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        SDL_Log("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    } else {
        //Convert surface to display format
        SDL_Surface *formattedSurface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_RGBA8888);
        if (formattedSurface == nullptr) {
            SDL_Log("Unable to convert loaded surface to display format! %s\n", SDL_GetError());
        } else {
            //Create blank streamable texture
            newTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_STREAMING, formattedSurface->w,
                                           formattedSurface->h);
            if (newTexture == nullptr) {
                SDL_Log("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
            } else {
                //Enable blending on texture
                SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

                //Lock texture for manipulation
                SDL_LockTexture(newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch);

                //Copy loaded/formatted surface pixels
                memcpy(mPixels, formattedSurface->pixels,
                       formattedSurface->pitch * formattedSurface->h);

                //Get image dimensions
                mWidth = formattedSurface->w;
                mHeight = formattedSurface->h;

                //Get pixel data in editable format
                auto *pixels = (Uint32 *) mPixels;
                int pixelCount = (mPitch / 4) * mHeight;

                //Map colors
                Uint32 colorKey = SDL_MapRGB(formattedSurface->format, 0, 0xFF, 0xFF);
                Uint32 transparent = SDL_MapRGBA(formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00);

                //Color key pixels
                for (int i = 0; i < pixelCount; ++i) {
                    if (pixels[i] == colorKey) {
                        pixels[i] = transparent;
                    }
                }

                //Unlock texture to update
                SDL_UnlockTexture(newTexture);
                mPixels = nullptr;
            }

            //Get rid of old formatted surface
            SDL_DestroySurface(formattedSurface);
        }

        //Get rid of old loaded surface
        SDL_DestroySurface(loadedSurface);
    }

    //Return success
    mTexture = newTexture;
    return mTexture != nullptr;
}

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
    if( textSurface != nullptr )
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
        if( mTexture == nullptr )
        {
            SDL_Log( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    else
    {
        SDL_Log( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }


    //Return success
    return mTexture != nullptr;
}
#endif

bool LTexture::createBlank(int width, int height, SDL_TextureAccess access) {
    //Create uninitialized texture
    mTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);
    if (mTexture == nullptr) {
        SDL_Log("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
    } else {
        mWidth = width;
        mHeight = height;
    }

    return mTexture != nullptr;
}

void LTexture::free() {
    //Free texture if it exists
    if (mTexture != nullptr) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
        mPixels = nullptr;
        mPitch = 0;
    }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
    //Modulate texture rgb
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
    //Set blending function
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
    //Modulate texture alpha
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center,
                      SDL_FlipMode flip) {
    //Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    //Set clip rendering dimensions
    if (clip != nullptr) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

void LTexture::setAsRenderTarget() {
    //Make self render target
    SDL_SetRenderTarget(gRenderer, mTexture);
}

int LTexture::getWidth() const {
    return mWidth;
}

int LTexture::getHeight() const {
    return mHeight;
}

bool LTexture::lockTexture() {
    bool success = true;

    //Texture is already locked
    if (mPixels != nullptr) {
        SDL_Log("Texture is already locked!\n");
        success = false;
    }
        //Lock texture
    else {
        if (SDL_LockTexture(mTexture, nullptr, &mPixels, &mPitch) != 0) {
            SDL_Log("Unable to lock texture! %s\n", SDL_GetError());
            success = false;
        }
    }

    return success;
}

bool LTexture::unlockTexture() {
    bool success = true;

    //Texture is not locked
    if (mPixels == nullptr) {
        SDL_Log("Texture is not locked!\n");
        success = false;
    }
        //Unlock texture
    else {
        SDL_UnlockTexture(mTexture);
        mPixels = nullptr;
        mPitch = 0;
    }

    return success;
}

void *LTexture::getPixels() {
    return mPixels;
}

void LTexture::copyPixels(void *pixels) {
    //Texture is locked
    if (mPixels != nullptr) {
        //Copy to locked pixels
        memcpy(mPixels, pixels, mPitch * mHeight);
    }
}

int LTexture::getPitch() const {
    return mPitch;
}

Uint32 LTexture::getPixel32(unsigned int x, unsigned int y) {
    //Convert the pixels to 32 bit
    auto *pixels = (Uint32 *) mPixels;

    //Get the pixel requested
    return pixels[(y * (mPitch / 4)) + x];
}

bool init() {
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    } else {
        //Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            SDL_Log("Warning: Linear texture filtering not enabled!");
        }

        //Get device display mode
        SDL_DisplayMode displayMode;
        if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
            gScreenRect.w = displayMode.w;
            gScreenRect.h = displayMode.h;
        }

        //Create window
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   gScreenRect.w, gScreenRect.h, SDL_WINDOW_SHOWN);
        if (gWindow == nullptr) {
            SDL_Log("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        } else {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == nullptr) {
                SDL_Log("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            } else {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }

    return success;
}

bool loadMedia() {
    //Loading success flag
    bool success = true;

    //Load splash texture
    if (!gSplashTexture.loadFromFile("preview.png")) {
        SDL_Log("Failed to load splash texture!\n");
        success = false;
    }

    return success;
}

void close() {
    //Free loaded images
    gSplashTexture.free();

    //Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;

    //Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char *args[]) {
    //Start up SDL and create window
    if (!init()) {
        SDL_Log("Failed to initialize!\n");
    } else {
        //Load media
        if (!loadMedia()) {
            SDL_Log("Failed to load media!\n");
        } else {
            //Main loop flag
            bool quit = false;

            //Event handler
            SDL_Event e;

            //While application is running
            while (!quit) {
                //Handle events on queue
                while (SDL_PollEvent(&e) != 0) {
                    //User requests quit
                    if (e.type == SDL_EVENT_QUIT) {
                        quit = true;
                    } else if (e.type == SDL_EVENT_KEY_DOWN) {
                        if ((e.key.keysym.sym == SDLK_AC_BACK) ||
                            (e.key.keysym.sym == SDLK_ESCAPE)) {
                            quit = true;
                        }
                    } else if (e.type == SDL_EVENT_FINGER_DOWN) {
                        SDL_Log("User press %f %f", e.tfinger.x, e.tfinger.y);
                    }
                }

                //Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                //Render splash
                gSplashTexture.render((gScreenRect.w - gSplashTexture.getWidth()) / 2,
                                      (gScreenRect.h - gSplashTexture.getHeight()) / 2);

                //Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}


#endif
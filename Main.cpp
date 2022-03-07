#include <SDL.h>
#include <SDL_image.h>
#include <cstdlib>
#include <string>
#include <time.h>

// Declare global variables
SDL_Window* window;
SDL_Renderer* renderer;
int width  = 640;
int height = 480;

bool init() {
    // initialize SDL and randomize RNG seed
    SDL_Init(SDL_INIT_EVERYTHING);
    srand(time(NULL));
    
    // create window and renderer
    window = SDL_CreateWindow("Dennis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    return true;
}

void kill() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void drawgradient(int x = 0, int y = 0, int w = width, int h = height, int r = 0, int g = 0, int b = 255) {

    // Gradient drawing function, can only draw from black to color
    // ----------------------------------------------------------
    // x, y, w, h: gradient coords,     e.g. "0, 0, 320, 240"
    // r, g, b: ending color to draw,   e.g. "255, 255, 128"

    for (int i = 0; i < h; i++) {
        SDL_SetRenderDrawColor(renderer, 
            trunc((i * r) / h), 
            trunc((i * g) / h), 
            trunc((i * b) / h), 
            255);

        SDL_RenderDrawLine(renderer, x, y + i, x+w, y + i);
    }
}

void drawtext(std::string text, int x = 0, int y = 0, int scale = 1) {

    // Bitmap font-drawing function, supports printable ASCII only
    // ----------------------------------------------------------
    // text: a std string,          e.g. "Hello World"
    // x, y: x and y coordinates,   e.g. "320, 240"
    // scale: scaling factor,       e.g. "2" for double-size text

    // printable ASCII (use this string for making new fonts):
    // !\"#$%'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
    SDL_Surface* surface = IMG_Load("res/font.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect src;
    SDL_Rect dest;

    int const char_width  = 12;
    int const char_height = surface->h;

    for (std::string::size_type i = 0; i < text.size(); ++i) {
        // get ASCII value of current character
        int char_value = text[i] - 32;

        // get character coords in source image
        // width and height are 1 character
        src.x = (char_value *char_width);
        src.y = 0;
        src.w = char_width;
        src.h = char_height;

        // get x and y coords, offset by current character count
        // width and height bound-box get scaled here as well
        dest.x = x + (i * (char_width * scale));
        dest.y = y;
        dest.w = char_width * scale;
        dest.h = char_height * scale;

        SDL_RenderCopy(renderer, texture, &src, &dest);
    }

    SDL_FreeSurface(surface);
}

bool test() {
    // Border-color-shifting animation
    // Kept for testing purposes and because it looks cool

    static int color = 0;
    static int offset_r = rand() % 255;
    static int offset_g = rand() % 255;
    static int offset_b = rand() % 255;

    SDL_SetRenderDrawColor(renderer,
        (color - offset_r) % 255,
        (color - offset_g) % 255,
        (color - offset_b) % 255,
        color);

    SDL_Rect rectangle;
    rectangle.x = 0 + (color * 2);
    rectangle.y = 0 + (color * 2);
    rectangle.w = width - (color * 4);
    rectangle.h = height - (color * 4);
    SDL_RenderFillRect(renderer, &rectangle);

    color += 1;

    if (rectangle.w <= 0 || rectangle.h <= 0) {
        // re-rolls offsets and resets animation
        offset_r = rand() % 255;
        offset_g = rand() % 255;
        offset_b = rand() % 255;
        color = 0;
    };

    drawtext("HELLO WORLD !", width / 4, height / 3, 2);
    drawtext("don't adjust your monitor", width / 4, (height / 2) + 20);
    drawtext("this is just a test!", width / 4, (height / 2) + 40);
    return true;
}

bool loop() {
    drawgradient(0, 0, width, height, 0, 0, 255);
    return true;
}

int main(int argc, char** args) {
    if (!init()) return 1;

    SDL_Event evt;
    bool programrunning = true;

    while (programrunning) {
        while (SDL_PollEvent(&evt)) {
            switch (evt.type) {
            case SDL_QUIT:  programrunning = false;   break;
            case SDL_WINDOWEVENT:
                if (evt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);

                    SDL_GetWindowSize(window, &width, &height);
                }
            }
        }
        loop();
        // render-updater placed here for simplicity's sake
        SDL_RenderPresent(renderer);
    }

    kill();
    return 0;
}

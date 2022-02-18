/* painter_wasm.c
 *   $ ./painter_wasm
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "emscripten.h"

#include "imageSynth.h"

/* PPM(P6) Read/Write snippets
 *
 * Adopted from:
 * https://github.com/skeeto/scratch/blob/master/animation/pixelsort.c
 */
struct ppm
{
    long width;
    long height;
    unsigned char data[];
};

static struct ppm *ppm_create(long width, long height)
{
    struct ppm *m = malloc(sizeof(*m) + width * height * 3);
    m->width = width;
    m->height = height;
    return m;
}

static struct ppm *ppm_read(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
    {
        perror(filename);
        return NULL;
    }
    struct ppm *m;
    long width, height;
    if (fscanf(f, "P6 %ld%ld%*d%*c", &width, &height) < 2)
    {
        fprintf(stderr, "Not a valid PPM image.\n");
        return NULL;
    }
    m = ppm_create(width, height);
    fread(m->data, width * height, 3, f);
    fclose(f);
    return m;
}

ImageBuffer source_buffer = {.data = NULL};
ImageBuffer mask_buffer = {.data = NULL};

SDL_Window *window = NULL;
SDL_Surface *screen_surface = NULL;
SDL_Surface *source_surface = NULL;
SDL_Surface *mask_surface = NULL;
SDL_Rect brush = {.x = 0, .y = 0, .w = 16, .h = 16};

int w = 0;
int h = 0;

unsigned char *mask_surface_data = NULL;
unsigned char *mask_buffer_data = NULL;

void looper()
{
    static int cancel = 0;
    static bool is_running = true;
    static bool pressed = false;
    static SDL_Event event;

    static TImageSynthParameters params = {
        .matchContextType = 1,
        .mapWeight = 0.5f,
        .sensitivityToOutliers = 0.117f,
        .patchSize = 30,
        .maxProbeCount = 200,
    };

    SDL_PollEvent(&event);

    switch (event.type)
    {
    case SDL_MOUSEMOTION:
        brush.x = event.motion.x - brush.w / 2;
        brush.y = event.motion.y - brush.h / 2;
        if (!pressed)
            memset(mask_surface_data, 0, w * h * 4 * sizeof(char));
        SDL_FillRect(mask_surface, &brush, SDL_MapRGB(mask_surface->format, 0xFF, 0xFF, 0xFF));
        break;
    case SDL_MOUSEBUTTONDOWN:
        pressed = true;
        SDL_FillRect(mask_surface, &brush, SDL_MapRGB(mask_surface->format, 0xFF, 0xFF, 0xFF));
        break;
    case SDL_MOUSEBUTTONUP:
        pressed = false;
        // Do the synthesis opration
        for (int i = 0; i < w * h; ++i)
            mask_buffer_data[i] = mask_surface_data[i * 4];
        imageSynth(&source_buffer, &mask_buffer, T_RGB, &params, NULL, NULL, &cancel);
        memset(mask_surface_data, 0, w * h * 4 * sizeof(char));
        break;
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_ESCAPE:
            // Quit
            is_running = false;
            break;
        case SDLK_r:
            // Reset source image
            break;
        case SDLK_LEFTBRACKET:
            // Increase brush size
            brush.w--;
            brush.h--;
            break;
        case SDLK_RIGHTBRACKET:
            // Decrease brush size
            brush.w++;
            brush.h++;
            break;
        }
        break;
    }

    SDL_BlitSurface(source_surface, NULL, screen_surface, NULL);
    SDL_BlitSurface(mask_surface, NULL, screen_surface, NULL);
    SDL_UpdateWindowSurface(window);

    if (!is_running)
        emscripten_cancel_main_loop();
}

int main(int argc, char **argv)
{
    const char *source_in = argc > 1 ? argv[1] : "assets/source001.ppm";
    struct ppm *source_ppm = ppm_read(source_in);

    if (source_ppm == NULL)
    {
        fprintf(stderr, "Source image not provided.\n");
        exit(EXIT_FAILURE);
    }

    w = source_ppm->width;
    h = source_ppm->width;

    mask_surface_data = malloc(w * h * 4 * sizeof(*mask_surface_data));
    mask_buffer_data = malloc(w * h * sizeof(*mask_buffer_data));

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "error %s \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("resynthesizer", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    screen_surface = SDL_GetWindowSurface(window);
    source_surface = SDL_CreateRGBSurfaceFrom(
        source_ppm->data, w, h, 24, w * 3, 0xFF, 0xFF00, 0xFF0000, 0);
    mask_surface = SDL_CreateRGBSurfaceFrom(
        mask_surface_data, w, h, 32, w * 4, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);

    source_buffer.data = source_ppm->data;
    source_buffer.width = w;
    source_buffer.height = h;
    source_buffer.rowBytes = w * 3;

    mask_buffer.data = mask_buffer_data;
    mask_buffer.width = w;
    mask_buffer.height = h;
    mask_buffer.rowBytes = w * 1;

    emscripten_set_main_loop(looper, 0, 1);

    SDL_FreeSurface(mask_surface);
    SDL_FreeSurface(source_surface);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free(mask_buffer_data);
    free(mask_surface_data);
    free(source_ppm);

    return EXIT_SUCCESS;
}

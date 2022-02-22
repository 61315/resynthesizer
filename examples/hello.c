// examples/hello.c
#include <stdlib.h>
#include <stdio.h>

#include "imageSynth.h"

#define WIDTH 256
#define HEIGHT 256

static unsigned char source_image_data[WIDTH * HEIGHT * 3];
static unsigned char mask_image_data[WIDTH * HEIGHT * 1];

int main()
{
    ImageBuffer source_image_buffer = {
        .data = source_image_data,
        .width = WIDTH,
        .height = HEIGHT,
        .rowBytes = WIDTH * 3,
    };

    ImageBuffer mask_image_buffer = {
        .data = mask_image_data,
        .width = WIDTH,
        .height = HEIGHT,
        .rowBytes = WIDTH * 1,
    };

    TImageSynthParameters params = {
        .matchContextType = 1,
        .mapWeight = 0.5f,
        .sensitivityToOutliers = 0.117f,
        .patchSize = 30,
        .maxProbeCount = 200,
    };

    // Fill the source buffer with a smoothly-colored square
    for (int i = 0; i < HEIGHT; ++i)
    {
        for (int j = 0; j < WIDTH; ++j)
        {
            source_image_data[(i * HEIGHT + j) * 3 + 0] = 255 - i;
            source_image_data[(i * HEIGHT + j) * 3 + 1] = i;
            source_image_data[(i * HEIGHT + j) * 3 + 2] = 255 - j;
        }
    }

    for (int i = 0; i < HEIGHT; ++i)
        for (int j = 0; j < WIDTH; ++j)
            if (WIDTH / 4 < j && j < WIDTH - WIDTH / 4 && HEIGHT / 4 < i && i < HEIGHT - HEIGHT / 4)
                mask_image_data[i * HEIGHT + j] = 0xFF;

    int error = 0;
    int cancel = 0;

    error = imageSynth(&source_image_buffer, &mask_image_buffer, T_RGB, &params, NULL, NULL, &cancel);

    const char *enums[] = {
        [IMAGE_SYNTH_SUCCESS] = "IMAGE_SYNTH_SUCCESS",
        [IMAGE_SYNTH_ERROR_INVALID_IMAGE_FORMAT] = "IMAGE_SYNTH_ERROR_INVALID_IMAGE_FORMAT",
        [IMAGE_SYNTH_ERROR_IMAGE_MASK_MISMATCH] = "IMAGE_SYNTH_ERROR_IMAGE_MASK_MISMATCH",
        [IMAGE_SYNTH_ERROR_PATCH_SIZE_EXCEEDED] = "IMAGE_SYNTH_ERROR_PATCH_SIZE_EXCEEDED",
        [IMAGE_SYNTH_ERROR_MATCH_CONTEXT_TYPE_RANGE] = "IMAGE_SYNTH_ERROR_MATCH_CONTEXT_TYPE_RANGE",
        [IMAGE_SYNTH_ERROR_EMPTY_TARGET] = "IMAGE_SYNTH_ERROR_EMPTY_TARGET",
        [IMAGE_SYNTH_ERROR_EMPTY_CORPUS] = "IMAGE_SYNTH_ERROR_EMPTY_CORPUS",
    };

    puts(enums[error]);

    return EXIT_SUCCESS;
}

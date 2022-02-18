// examples/hello.c
#include <stdlib.h>
#include <stdio.h>

#include "imageSynth.h"

int main()
{
    TImageSynthParameters *params = malloc(sizeof(TImageSynthParameters));
    ImageBuffer *source_image_buffer = malloc(sizeof(ImageBuffer));
    ImageBuffer *mask_image_buffer = malloc(sizeof(ImageBuffer));

    int cancel_token = 0;
    int error = 0;

    error = imageSynth(source_image_buffer, mask_image_buffer, T_RGB, params, NULL, NULL, &cancel_token);

    // We're sending empty source/mask buffers.
    // imageSynth() should return `IMAGE_SYNTH_ERROR_EMPTY_TARGET`
    if (error == IMAGE_SYNTH_ERROR_EMPTY_TARGET)
        puts("hello");

    return EXIT_SUCCESS;
}

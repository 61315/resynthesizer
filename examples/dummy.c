// examples/dummy.c
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

    if (error != 0)
    {
        printf("Task failed with the error code: %d\n", error);
        exit(EXIT_FAILURE);
    }

    printf("Task finished\n");

    return EXIT_SUCCESS;
}

// examples/ppm.c
#include <stdlib.h>
#include <stdio.h>

#include <imageSynth.h>

/* PPM(P6) Read/Write snippets
 * 
 * Borrowed and modified from:
 * https://github.com/skeeto/scratch/blob/master/animation/pixelsort.c
 */
struct ppm
{
    long width;
    long height;
    unsigned char data[];
};

static struct ppm *
ppm_create(long width, long height)
{
    struct ppm *m = malloc(sizeof(*m) + width * height * 3);
    m->width = width;
    m->height = height;
    return m;
}

static void
ppm_write(struct ppm *m, FILE *f)
{
    fprintf(f, "P6\n%ld %ld\n255\n", m->width, m->height);
    if (!fwrite(m->data, m->width * m->height, 3, f))
        exit(EXIT_FAILURE);
}

static struct ppm *
ppm_read(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    struct ppm *m;
    long width, height;
    if (fscanf(f, "P6 %ld%ld%*d%*c", &width, &height) < 2)
        return NULL;
    m = ppm_create(width, height);
    fread(m->data, width * height, 3, f);
    fclose(f);
    return m;
}

int main()
{
#ifdef _WIN32
    int _setmode(int, int);
    _setmode(0, 0x8000);
    _setmode(1, 0x8000);
#endif
    struct ppm *source_ppm = ppm_read("assets/source.ppm");
    struct ppm *mask_ppm = ppm_read("assets/mask.ppm");

    unsigned char *mask_grayscale = malloc(mask_ppm->width * mask_ppm->height);

    // Naive conversion of a PPM image to a grayscale bitmap
    for (int i = 0; i < mask_ppm->width * mask_ppm->height; ++i)
    {
        mask_grayscale[i] = mask_ppm->data[i * 3]; // | mask_ppm->data[i * 3 + 1] | mask_ppm->data[i * 3 + 2];
    }

    ImageBuffer source_buffer = {
        .data = source_ppm->data,
        .width = source_ppm->width,
        .height = source_ppm->height,
        .rowBytes = source_ppm->width * 3,
    };

    ImageBuffer mask_buffer = {
        .data = mask_grayscale,
        .width = mask_ppm->width,
        .height = mask_ppm->height,
        .rowBytes = mask_ppm->width,
    };

    TImageSynthParameters params = {
        .isMakeSeamlesslyTileableHorizontally = FALSE,
        .isMakeSeamlesslyTileableVertically = FALSE,
        .matchContextType = 2,
        .mapWeight = 0.5f,
        .sensitivityToOutliers = 0.117f,
        .patchSize = 9,
        .maxProbeCount = 50,
    };

    int cancel_token = 0;
    int error = 0;

    error = imageSynth(&source_buffer, &mask_buffer, T_RGB, &params, NULL, NULL, &cancel_token);

    printf("Task finished with the error code: %d\n", error);

    FILE *file_result = fopen("ppm_result.ppm", "wb");
    ppm_write(source_ppm, file_result);
    fclose(file_result);

    free(source_ppm);
    free(mask_ppm);
    free(mask_grayscale);

    return EXIT_SUCCESS;
}

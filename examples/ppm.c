/* examples/ppm.c
 *   $ ./ppm source! mask! result? context? neighbors? probes?
 */
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
    if (f == NULL)
    {
        perror(filename);
        return NULL;
    }
    struct ppm *m;
    long width, height;
    if (fscanf(f, "P6 %ld%ld%*d%*c", &width, &height) < 2)
        return NULL;
    m = ppm_create(width, height);
    fread(m->data, width * height, 3, f);
    fclose(f);
    return m;
}

int main(int argc, char **argv)
{
    const char *source_in   = argc > 1 ? argv[1] : NULL;
    const char *mask_in     = argc > 2 ? argv[2] : NULL;
    const char *result_out  = argc > 3 ? argv[3] : "out.ppm";
    const int context       = argc > 4 ? atoi(argv[4]) : 1;
    const int neighbors     = argc > 5 ? atoi(argv[5]) : 9;
    const int probes        = argc > 6 ? atoi(argv[6]) : 64;

    printf("context: %d neighbors: %d probes: %d\n", context, neighbors, probes);
#ifdef _WIN32
    int _setmode(int, int);
    _setmode(0, 0x8000);
    _setmode(1, 0x8000);
#endif
    struct ppm *source_ppm = ppm_read(source_in);
    struct ppm *mask_ppm = ppm_read(mask_in);

    if (source_ppm == NULL || mask_ppm == NULL)
    {
        fprintf(stderr, "Source or mask image not provided.\n");
        exit(EXIT_FAILURE);
    }

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
        .matchContextType = context,
        .mapWeight = 0.5f,
        .sensitivityToOutliers = 0.117f,
        .patchSize = neighbors,
        .maxProbeCount = probes,
    };

    int cancel_token = 0;
    int error = 0;

    error = imageSynth(&source_buffer, &mask_buffer, T_RGB, &params, NULL, NULL, &cancel_token);

    if (error != 0)
        fprintf(stderr, "Operation failed. Error code: %d\n", error);

    FILE *result_file = fopen(result_out, "wb");
    ppm_write(source_ppm, result_file);
    fclose(result_file);

    free(source_ppm);
    free(mask_ppm);
    free(mask_grayscale);

    return EXIT_SUCCESS;
}

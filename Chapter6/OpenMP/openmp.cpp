#include <complex>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <omp.h>
using namespace std;

static const int WIDTH = 1920;
static const int HEIGHT = 1080;
static const int MAX_VALUE = 255;

static const float ORIGIN_X = WIDTH * 2 / 3;
static const float ORIGIN_Y = HEIGHT / 2;

typedef struct
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} RGB;

static const RGB PINK = { 0XFF, 0XCC, 0XCC };

static unsigned char iteration(int x, int y)
{
    complex<float> c((x - ORIGIN_X) / (WIDTH / 3), (ORIGIN_Y - y) / (HEIGHT / 2));
    complex<float> z = 0;

    for (int i = 0; i <= MAX_VALUE; i++)
    {
        /* basic formula */
        z = z * z + c;
        if (z.real() > 2 || z.imag() > 2)
            return (unsigned char)i;
    }
    return (unsigned char)0;
}

static void saveAsPpm(const RGB *const array)
{
    FILE *f = fopen("mandelbrot.ppm", "wb+");
    fprintf(f,
            "P6\n" /* PPM magic number */
            "#Mandelbrot Set\n"
            "%d "   /* width, in ASCII decimal */
            "%d\n"  /* height, in ASCII decimal */
            "%d\n", /* maximum color value, in ASCII decimal */
            WIDTH, HEIGHT, MAX_VALUE);
    fwrite(array, sizeof(RGB), WIDTH * HEIGHT, f);
    fclose(f);
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "用法：openmp.exe <线程数量>\n");
        return EXIT_FAILURE;
    }
    const int threads = atoi(argv[1]);

    RGB *array = new RGB[WIDTH * HEIGHT];

#pragma omp barrier
    const double start = omp_get_wtime();
#pragma omp parallel for num_threads(threads)
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
        {
            unsigned char iter = iteration(j, i);
            if (iter)
            {
                const RGB pixel = {
                    iter,
                    fabsf(j - ORIGIN_X) / WIDTH * MAX_VALUE,
                    fabsf(i - ORIGIN_Y) / HEIGHT * MAX_VALUE };
                array[i * WIDTH + j] = pixel;
            }
            else
                array[i * WIDTH + j] = PINK;
        }
    cout << setprecision(7) << omp_get_wtime() - start << endl;

    saveAsPpm(array);
    delete[] array;
    return 0;
}
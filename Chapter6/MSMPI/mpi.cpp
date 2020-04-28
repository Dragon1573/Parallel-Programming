#include <complex>
#include <cmath>
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
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

static const RGB PINK = {0xFF, 0xCC, 0xCC};

static unsigned char iteration(const int x, const int y)
{
    complex<float> c((x - ORIGIN_X) / (WIDTH / 3), (y - ORIGIN_Y) / (HEIGHT / 2));
    complex<float> z = 0;

    for (int i = 0; i <= MAX_VALUE; ++i)
    {
        z = z * z + c;
        if (z.real() > 2 || z.imag() > 2)
            return (unsigned char)i;
    }
    return (unsigned char)0;
}

static void saveAsPpm(const RGB *const array)
{
    FILE *file = fopen("mandelbrot.ppm", "wb+");
    fprintf(file,
            "P6\n"
            "#Mandelbrot Set\n"
            "%d %d\n"
            "%d\n",
            WIDTH, HEIGHT, MAX_VALUE);
    fwrite(array, sizeof(RGB), WIDTH * HEIGHT, file);
    fclose(file);
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int proc_count;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    const double start = MPI_Wtime();

    RGB *local_arr = (RGB *)calloc(WIDTH * HEIGHT / proc_count, sizeof(RGB));
    const int calcs_per_proc = HEIGHT / proc_count;
    const int calc_start = proc_rank * calcs_per_proc;
    for (int i = calc_start; i < (calc_start + calcs_per_proc); ++i)
    {
        for (int j = 0; j < WIDTH; ++j)
        {
            unsigned char iter = iteration(j, i);
            if (iter)
            {
                const RGB pixel = {
                    iter,
                    fabsf(j - ORIGIN_X) / WIDTH * MAX_VALUE,
                    fabsf(i - ORIGIN_Y) / HEIGHT * MAX_VALUE};
                local_arr[(i - calc_start) * WIDTH + j] = pixel;
            }
            else
            {
                local_arr[(i - calc_start) * WIDTH + j] = PINK;
            }
        }
    }

    RGB *array = (RGB *)calloc(WIDTH * HEIGHT, sizeof(RGB));
    MPI_Gather(local_arr,                       // 数据源（本地局部矩阵）
               WIDTH * HEIGHT / proc_count * 3, // 发送数据量（矩阵大小，每个元素是3个U_CHAR）
               MPI_UNSIGNED_CHAR,               // 发送类型
               array,                           // 目标变量（整体矩阵）
               WIDTH * HEIGHT / proc_count * 3, // 接收数据量（矩阵大小，每个元素时3个U_CHAR）
               MPI_UNSIGNED_CHAR,               // 接收类型
               0,                               // 接收进程
               MPI_COMM_WORLD);                 // 通信子

    if (proc_rank == 0)
    {
        cout << setprecision(7) << MPI_Wtime() - start << endl;
        saveAsPpm(array);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}

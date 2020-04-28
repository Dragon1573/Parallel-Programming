#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

typedef int *int_p;
const int INT = sizeof(int);

/// <summary>矩阵－向量乘法</summary>
/// <param name="threads">线程数量</param>
/// <param name="r">矩阵行数</param>
/// <param name="c">矩阵列数</param>
/// <param name="mat">矩阵</param>
/// <param name="vec">向量</param>
/// <param name="res">结果</param>
void multiplication(const int threads, const long long r, const long long c,
                    int *mat, int *vec, int *res)
{
    long long i, j;
    // 并行矩阵生成
#pragma omp parallel for num_threads(threads) \
 default(none) private(i, j) shared(mat, r, c)
    for (i = 0; i < r; i++)
    {
        for (j = 0; j < c; j++)
        {
            mat[i * c + j] = rand() % 1024 + 1;
        }
    }

    // 并行向量生成
#pragma omp parallel for num_threads(threads) \
 default(none) private(i, j) shared(vec, r, c)
    for (j = 0; j < c; j++)
    {
        vec[j] = rand() % 1024 + 1;
    }

    // 并行线性代数乘
#pragma omp parallel for num_threads(threads) \
 default(none) private(i, j) shared(mat, vec, res, r, c)
    for (i = 0; i < r; i++)
    {
        int temp = 0;
        for (j = 0; j < c; j++)
        {
            temp += mat[i * c + j] * vec[j];
        }
        res[i] = temp;
    }
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "用法：Experiment4-2.exe <线程数量>\n\n");
        return EXIT_FAILURE;
    }

    // 线程数量
    const int threads = atoi(argv[1]);
    // 矩阵行数
    const long long rows;
    scanf("%lld%*c", &rows);
    // 矩阵列数
    const long long columns;
    scanf("%lld%*c", &columns);
    // 矩阵
    int_p matrix = (int_p)calloc(rows * columns, INT);
    // 向量
    int_p vector = (int_p)calloc(columns, INT);
    // 计算结果
    int_p result = (int_p)calloc(rows, INT);

    // 并行运算并计时
#pragma omp barrier
    const double start = omp_get_wtime();
    srand(0x6A1B);
    multiplication(threads, rows, columns, matrix, vector, result);
    const double duration = omp_get_wtime() - start;

    // 计时输出
    printf("%.9lf\n", duration);

    return EXIT_SUCCESS;
}
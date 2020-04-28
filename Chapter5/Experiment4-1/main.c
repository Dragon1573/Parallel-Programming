#include <omp.h>
#include <stdlib.h>
#include <stdio.h>

typedef int *int_p;
#define INT sizeof(int)

/// <summary>并行奇偶交换排序算法</summary>
/// <param name="sort_array">待排序数组</param>
/// <param name="threads">线程数量</param>
/// <param name="elements">元素数量</param>
/// <param name="output">输出模式</param>
void parallel_sorter(int *sort_array,
                     const int *const threads,
                     const int *const elements,
                     const int *const output)
{
    // 设置随机种子
    const double seed = omp_get_wtime();
    srand(*(unsigned int *)&seed);

    int i;
    // 并行元素生成
#pragma omp parallel for num_threads(*threads) private(i)
    for (i = 0; i < *elements; i++)
    {
        // 限制元素大小在 [1, 65536] 之间
        *(sort_array + i) = rand() % 65536 + 1;
    }

    // 并行奇偶交换排序
    int phase;
#pragma omp parallel num_threads(*threads) \
 default(none) shared(sort_array, threads) private(i, phase)
    for (phase = 0; phase < *elements; phase++)
    {
        if (phase % 2 == 0)
        {
#pragma omp for
            for (i = 1; i < *elements; i += 2)
            {
                if (sort_array[i - 1] > sort_array[i])
                {
                    sort_array[i - 1] ^= sort_array[i];
                    sort_array[i] = sort_array[i - 1] ^ sort_array[i];
                    sort_array[i - 1] ^= sort_array[i];
                }
            }
        }
        else
        {
#pragma omp for
            for (i = 0; i < *elements - 1; i += 2)
            {
                if (sort_array[i] > sort_array[i + 1])
                {
                    sort_array[i] ^= sort_array[i + 1];
                    sort_array[i + 1] = sort_array[i] ^ sort_array[i + 1];
                    sort_array[i] ^= sort_array[i + 1];
                }
            }
        }
    }
}

int main(int argc, const char *argv[])
{
    // 语法检查
    if (argc < 3)
    {
        fprintf(stderr, "用法：Experiment4-1.exe <线程数量> <元素数量> <是否输出>");
        return EXIT_FAILURE;
    }
    // 线程数量
    const int threads = atoi(argv[1]);
    // 元素数量
    const int elements = atoi(argv[2]);
    // 输出模式
    const int output = atoi(argv[3]);
    // 排序数组
    int_p sort_array = (int_p)calloc(elements, INT);

    // 排序
#pragma omp barrier
    const double start = omp_get_wtime();
    parallel_sorter(sort_array, &threads, &elements, &output);
    const double diff = omp_get_wtime() - start;

#pragma omp critical
    if (output)
    {
        for (int j = 0; j < elements; j++)
        {
            printf("%5d ", *(sort_array + j));
            if (j % 15 == 14)
            {
                printf("\n");
            }
        }
        printf("\n");
    }
    printf("%.9lf\n", diff);

    return EXIT_SUCCESS;
}
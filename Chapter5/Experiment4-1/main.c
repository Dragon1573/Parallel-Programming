#include <omp.h>
#include <stdlib.h>
#include <stdio.h>

typedef int *int_p;
#define INT sizeof(int)

/// <summary>������ż���������㷨</summary>
/// <param name="sort_array">����������</param>
/// <param name="threads">�߳�����</param>
/// <param name="elements">Ԫ������</param>
/// <param name="output">���ģʽ</param>
void parallel_sorter(int *sort_array,
                     const int *const threads,
                     const int *const elements,
                     const int *const output)
{
    // �����������
    const double seed = omp_get_wtime();
    srand(*(unsigned int *)&seed);

    int i;
    // ����Ԫ������
#pragma omp parallel for num_threads(*threads) private(i)
    for (i = 0; i < *elements; i++)
    {
        // ����Ԫ�ش�С�� [1, 65536] ֮��
        *(sort_array + i) = rand() % 65536 + 1;
    }

    // ������ż��������
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
    // �﷨���
    if (argc < 3)
    {
        fprintf(stderr, "�÷���Experiment4-1.exe <�߳�����> <Ԫ������> <�Ƿ����>");
        return EXIT_FAILURE;
    }
    // �߳�����
    const int threads = atoi(argv[1]);
    // Ԫ������
    const int elements = atoi(argv[2]);
    // ���ģʽ
    const int output = atoi(argv[3]);
    // ��������
    int_p sort_array = (int_p)calloc(elements, INT);

    // ����
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
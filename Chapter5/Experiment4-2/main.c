#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

typedef int *int_p;
const int INT = sizeof(int);

/// <summary>���������˷�</summary>
/// <param name="threads">�߳�����</param>
/// <param name="r">��������</param>
/// <param name="c">��������</param>
/// <param name="mat">����</param>
/// <param name="vec">����</param>
/// <param name="res">���</param>
void multiplication(const int threads, const long long r, const long long c,
                    int *mat, int *vec, int *res)
{
    long long i, j;
    // ���о�������
#pragma omp parallel for num_threads(threads) \
 default(none) private(i, j) shared(mat, r, c)
    for (i = 0; i < r; i++)
    {
        for (j = 0; j < c; j++)
        {
            mat[i * c + j] = rand() % 1024 + 1;
        }
    }

    // ������������
#pragma omp parallel for num_threads(threads) \
 default(none) private(i, j) shared(vec, r, c)
    for (j = 0; j < c; j++)
    {
        vec[j] = rand() % 1024 + 1;
    }

    // �������Դ�����
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
        fprintf(stderr, "�÷���Experiment4-2.exe <�߳�����>\n\n");
        return EXIT_FAILURE;
    }

    // �߳�����
    const int threads = atoi(argv[1]);
    // ��������
    const long long rows;
    scanf("%lld%*c", &rows);
    // ��������
    const long long columns;
    scanf("%lld%*c", &columns);
    // ����
    int_p matrix = (int_p)calloc(rows * columns, INT);
    // ����
    int_p vector = (int_p)calloc(columns, INT);
    // ������
    int_p result = (int_p)calloc(rows, INT);

    // �������㲢��ʱ
#pragma omp barrier
    const double start = omp_get_wtime();
    srand(0x6A1B);
    multiplication(threads, rows, columns, matrix, vector, result);
    const double duration = omp_get_wtime() - start;

    // ��ʱ���
    printf("%.9lf\n", duration);

    return EXIT_SUCCESS;
}
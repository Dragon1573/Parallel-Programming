#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// ˫��������ռ�ֽ���
#define INT sizeof(int)
// ˫������ָ����д
#define int_p int *

int main(int argc, const char *argv[])
{

    // ���л�����ʼ��
    MPI_Init(&argc, &argv);

    // �߳�����
    int processCount;
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    // �̱߳��
    int processRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

    // �﷨���
    if (argc < 4)
    {
        if (processRank == 0)
            fprintf(stderr,
                    "\n�÷�: mpiexec -n $PROC_COUNT $PATH\\Experiment2-1.exe" \
                    " $ROW $COLUMN $DISPLAY_TAG\n");

        MPI_Finalize();
        return -1;
    }

    // ��ȡ�����ģ
    size_t local_m = atoll(argv[1]), local_n = atoll(argv[2]);

    // �����������
    srand(time(NULL));

    // ���ɾ���
    int_p matrix = (int_p)calloc(local_m * local_n, INT);
    if (processRank == 0)
    {
        if (atoi(argv[3]))
            printf("����A��\n\n");
        for (int i = 0; i < local_m; i++)
        {
            for (int j = 0; j < local_n; j++)
            {
                // ����ÿ��Ԫ�ص�ֵ���ڱ����� [1, 16] ֮��
                matrix[local_m * i + j] = (int)(rand() % 16) + 1;
                if (atoi(argv[3]))
                    printf("%4d ", matrix[local_m * i + j]);
            }
            if (atoi(argv[3]))
                printf("\n");
        }
        if (atoi(argv[3]))
            printf("\n");
    }
    // ʹ��ɢ�佫��������л��֣������͵�����������
    int_p localMat = (int_p)calloc(local_m * local_n / processCount, INT);
    MPI_Scatter(matrix, local_m * local_n / processCount, MPI_INT, localMat,
                local_m * local_n / processCount, MPI_INT, 0, MPI_COMM_WORLD);

    // ���ɲ���������
    int_p localVec = (int_p)calloc(local_n / processCount, INT);
    for (int i = 0; i < local_n / processCount; i++)
        localVec[i] = (int)(rand() % 16) + 1;
    // ʹ��ȫ�־ۺ�ƴ�ϵõ�������������
    int_p vector = (int_p)calloc(local_n, INT);
    MPI_Allgather(localVec, local_n / processCount, MPI_INT, vector,
                  local_n / processCount, MPI_INT, MPI_COMM_WORLD);
    if (processRank == 0 && atoi(argv[3]))
    {
        printf("����x��\n");
        for (int i = 0; i < local_n; i++)
            printf("%4d ", vector[i]);
        printf("\n\n");
    }

    // ���㱾���̵ķֿ����-�����˷�
    int_p localRes = (int_p)calloc(local_m / processCount, INT);
    for (int i = 0; i < local_m / processCount; i++)
        for (int j = 0; j < local_n; j++)
            localRes[i] += localMat[i * local_n + j] * vector[j];

    // ���ܸ��ֿ�ļ�����
    int_p result = (int_p)calloc(local_m, INT);
    MPI_Gather(localRes, local_m / processCount, MPI_INT, result,
               local_m / processCount, MPI_INT, 0, MPI_COMM_WORLD);

    if (processRank == 0 && atoi(argv[3]))
    {
        // ��ӡ������
        printf("�˻�y��\n");
        for (int i = 0; i < local_m; i++)
            printf("%4d ", result[i]);
        printf("\n\n");
    }

    // �˳����л���
    MPI_Finalize();

    return 0;
}

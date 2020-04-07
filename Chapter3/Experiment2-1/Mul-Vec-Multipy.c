#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// 双长整型所占字节数
#define INT sizeof(int)
// 双长整型指针缩写
#define int_p int *

int main(int argc, const char *argv[])
{

    // 并行环境初始化
    MPI_Init(&argc, &argv);

    // 线程总数
    int processCount;
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    // 线程编号
    int processRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

    // 语法检查
    if (argc < 4)
    {
        if (processRank == 0)
            fprintf(stderr,
                    "\n用法: mpiexec -n $PROC_COUNT $PATH\\Experiment2-1.exe" \
                    " $ROW $COLUMN $DISPLAY_TAG\n");

        MPI_Finalize();
        return -1;
    }

    // 获取矩阵规模
    size_t local_m = atoll(argv[1]), local_n = atoll(argv[2]);

    // 设置随机种子
    srand(time(NULL));

    // 生成矩阵
    int_p matrix = (int_p)calloc(local_m * local_n, INT);
    if (processRank == 0)
    {
        if (atoi(argv[3]))
            printf("矩阵A：\n\n");
        for (int i = 0; i < local_m; i++)
        {
            for (int j = 0; j < local_n; j++)
            {
                // 矩阵每个元素的值将在闭区间 [1, 16] 之间
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
    // 使用散射将矩阵进行行划分，并发送到其他进程上
    int_p localMat = (int_p)calloc(local_m * local_n / processCount, INT);
    MPI_Scatter(matrix, local_m * local_n / processCount, MPI_INT, localMat,
                local_m * local_n / processCount, MPI_INT, 0, MPI_COMM_WORLD);

    // 生成部分列向量
    int_p localVec = (int_p)calloc(local_n / processCount, INT);
    for (int i = 0; i < local_n / processCount; i++)
        localVec[i] = (int)(rand() % 16) + 1;
    // 使用全局聚合拼合得到完整的列向量
    int_p vector = (int_p)calloc(local_n, INT);
    MPI_Allgather(localVec, local_n / processCount, MPI_INT, vector,
                  local_n / processCount, MPI_INT, MPI_COMM_WORLD);
    if (processRank == 0 && atoi(argv[3]))
    {
        printf("向量x：\n");
        for (int i = 0; i < local_n; i++)
            printf("%4d ", vector[i]);
        printf("\n\n");
    }

    // 计算本进程的分块矩阵-向量乘法
    int_p localRes = (int_p)calloc(local_m / processCount, INT);
    for (int i = 0; i < local_m / processCount; i++)
        for (int j = 0; j < local_n; j++)
            localRes[i] += localMat[i * local_n + j] * vector[j];

    // 汇总各分块的计算结果
    int_p result = (int_p)calloc(local_m, INT);
    MPI_Gather(localRes, local_m / processCount, MPI_INT, result,
               local_m / processCount, MPI_INT, 0, MPI_COMM_WORLD);

    if (processRank == 0 && atoi(argv[3]))
    {
        // 打印计算结果
        printf("乘积y：\n");
        for (int i = 0; i < local_m; i++)
            printf("%4d ", result[i]);
        printf("\n\n");
    }

    // 退出并行环境
    MPI_Finalize();

    return 0;
}

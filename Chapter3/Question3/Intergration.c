#include <stdio.h>
#include <math.h>
#include <mpi.h>

int PARTITIONS;

/// <summary>积分函数</summary>
/// <param name="x">自变量</param>
double mathFunc(double x)
{
    return 4.0 / (1 + pow(x, 2.0));
}

/// <summary>串行计算梯形积分</summary>
/// <param name="left">积分下限</param>
/// <param name="right">积分上限</param>
/// <param name="dHeight">微分长度</param>
double trapezoid(double left, double right, double dHeight)
{
    // 积分值
    double integration = (mathFunc(left) + mathFunc(right)) / 2.0;

    // 累计求积分
    for (int i = 1; i < PARTITIONS; i++)
    {
        double x = left + i * dHeight;
        integration += mathFunc(x);
    }
    integration *= dHeight;

    // 返回积分值
    return integration;
}

int main(int argc, const char *argv[])
{
    // 进程总数
    int processCount;
    // 进程编号
    int processRank;

    // 初始化并行环境
    MPI_Init(&argc, &argv);
    // 获取并行进程总数
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    // 获取当前进程编号
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

    // 设置每核心分片数
    PARTITIONS = atof(argv[1]);

    /*
     * 在已知积分区间的前提下，不需要从外部获取，
     * 此处，我们将积分区间按进程总数划分为n个并行梯形，
     * 每个进程再将自己的大区划分为512个微分梯形，
     * 这样就不存在总微分数量与进程数量无法整除导致任务分配异常的问题。
     */
     // 微分高度
    double h = 1.0 / (processCount * (double)PARTITIONS);
    // 本进程积分起点
    double a = processRank * (double)PARTITIONS * h;
    // 本进程积分终点
    double b = a + (double)PARTITIONS * h;
    // 本进程积分值
    double myIntegrated = trapezoid(a, b, h);
    // 全积分域累计值
    double total;

    // 并行区域
    if (processRank == 0)
    {
        // 进程0计算自己的任务片
        total = myIntegrated;
        for (int source = 1; source < processCount; source++)
        {
            // 接收其他进程获得的分片结果
            MPI_Recv(&myIntegrated, 1, MPI_DOUBLE, source,
                     0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // 分片结果累加
            total += myIntegrated;
        }
    }
    else
    {
        // 其他进程向进程0发送计算结果
        MPI_Send(&myIntegrated, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    // 由进程0负责输出
    if (processRank == 0)
    {
        printf(
            "With %d (process) × %d (partions / process) = %d (partitions),\n",
            processCount, PARTITIONS, processCount * PARTITIONS
            );
        printf("We integrated out Pi is approxy %.22lf.\n", total);
    }

    // 清理并行环境
    MPI_Finalize();

    return 0;
}
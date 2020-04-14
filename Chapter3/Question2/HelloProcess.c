#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define MAX_STRING (100)

int main()
{
    // 用于存储进程之间的消息
    char message[MAX_STRING];
    // 进程数量
    int process_counts;
    // 进程编号
    int process_rank;

    // 初始化 MPI 并行环境
    MPI_Init(NULL, NULL);
    // 由全进程通信子统计进程总数
    MPI_Comm_size(MPI_COMM_WORLD, &process_counts);
    // 由全进程通信子分配进程编号
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if (process_rank == 0)
    {
        /* 0号进程任务 */

        // 创建消息
        sprintf(message, "Hello, I'm process %d.", process_rank);
        // 向其他下一进程发送消息
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, process_rank + 1, 0, MPI_COMM_WORLD);
    }
    else if (process_counts - 1 == process_rank)
    {
        /* 最后一个进程 */

        // 接收上一进程的消息
        MPI_Recv(
            message, MAX_STRING, MPI_CHAR, process_rank - 1,
            0, MPI_COMM_WORLD, MPI_STATUS_IGNORE
            );
        // 打印消息
        printf(
            "I'm process %d, I've recieved message from process %d.\n",
            process_rank, process_rank - 1
            );
        // 不能再向后发送消息了
    }
    else
    {
        /* 其他进程 */

        // 接收上一进程的消息
        MPI_Recv(
            message, MAX_STRING, MPI_CHAR, process_rank - 1,
            0, MPI_COMM_WORLD, MPI_STATUS_IGNORE
            );
        // 打印消息
        printf(
            "I'm process %d, I've received message from process %d.\n",
            process_rank, process_rank - 1
            );
        // 创建消息
        sprintf(message, "Hello, I'm process %d.", process_rank);
        // 向其他下一进程发送消息
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, process_rank + 1, 0, MPI_COMM_WORLD);
    }

    // 结束并行环境
    MPI_Finalize();

    return 0;
}

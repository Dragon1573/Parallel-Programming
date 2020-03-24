#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define MAX_STRING (100)

int main()
{
    // ���ڴ洢����֮�����Ϣ
    char message[MAX_STRING];
    // ��������
    int process_counts;
    // ���̱��
    int process_rank;

    // ��ʼ�� MPI ���л���
    MPI_Init(NULL, NULL);
    // ��ȫ����ͨ����ͳ�ƽ�������
    MPI_Comm_size(MPI_COMM_WORLD, &process_counts);
    // ��ȫ����ͨ���ӷ�����̱��
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if (process_rank == 0)
    {
        /* 0�Ž������� */

        // ������Ϣ
        sprintf(message, "Hello, I'm process %d.", process_rank);
        // ��������һ���̷�����Ϣ
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, process_rank + 1, 0, MPI_COMM_WORLD);
    }
    else if (process_counts - 1 == process_rank)
    {
        /* ���һ������ */

        // ������һ���̵���Ϣ
        MPI_Recv(
            message, MAX_STRING, MPI_CHAR, process_rank - 1,
            0, MPI_COMM_WORLD, MPI_STATUS_IGNORE
            );
        // ��ӡ��Ϣ
        printf(
            "I'm process %d, I've recieved message from process %d.\n",
            process_rank, process_rank - 1
            );
        // �������������Ϣ��
    }
    else
    {
        /* �������� */

        // ������һ���̵���Ϣ
        MPI_Recv(
            message, MAX_STRING, MPI_CHAR, process_rank - 1,
            0, MPI_COMM_WORLD, MPI_STATUS_IGNORE
            );
        // ��ӡ��Ϣ
        printf(
            "I'm process %d, I've received message from process %d.\n",
            process_rank, process_rank - 1
            );
        // ������Ϣ
        sprintf(message, "Hello, I'm process %d.", process_rank);
        // ��������һ���̷�����Ϣ
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, process_rank + 1, 0, MPI_COMM_WORLD);
    }

    // �������л���
    MPI_Finalize();

    return 0;
}

#include <stdio.h>
#include <math.h>
#include <mpi.h>

int PARTITIONS;

/// <summary>���ֺ���</summary>
/// <param name="x">�Ա���</param>
double mathFunc(double x)
{
    return 4.0 / (1 + pow(x, 2.0));
}

/// <summary>���м������λ���</summary>
/// <param name="left">��������</param>
/// <param name="right">��������</param>
/// <param name="dHeight">΢�ֳ���</param>
double trapezoid(double left, double right, double dHeight)
{
    // ����ֵ
    double integration = (mathFunc(left) + mathFunc(right)) / 2.0;

    // �ۼ������
    for (int i = 1; i < PARTITIONS; i++)
    {
        double x = left + i * dHeight;
        integration += mathFunc(x);
    }
    integration *= dHeight;

    // ���ػ���ֵ
    return integration;
}

int main(int argc, const char *argv[])
{
    // ��������
    int processCount;
    // ���̱��
    int processRank;

    // ��ʼ�����л���
    MPI_Init(&argc, &argv);
    // ��ȡ���н�������
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    // ��ȡ��ǰ���̱��
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

    // ����ÿ���ķ�Ƭ��
    PARTITIONS = atof(argv[1]);

    /*
     * ����֪���������ǰ���£�����Ҫ���ⲿ��ȡ��
     * �˴������ǽ��������䰴������������Ϊn���������Σ�
     * ÿ�������ٽ��Լ��Ĵ�������Ϊ512��΢�����Σ�
     * �����Ͳ�������΢����������������޷�����������������쳣�����⡣
     */
     // ΢�ָ߶�
    double h = 1.0 / (processCount * (double)PARTITIONS);
    // �����̻������
    double a = processRank * (double)PARTITIONS * h;
    // �����̻����յ�
    double b = a + (double)PARTITIONS * h;
    // �����̻���ֵ
    double myIntegrated = trapezoid(a, b, h);
    // ȫ�������ۼ�ֵ
    double total;

    // ��������
    if (processRank == 0)
    {
        // ����0�����Լ�������Ƭ
        total = myIntegrated;
        for (int source = 1; source < processCount; source++)
        {
            // �����������̻�õķ�Ƭ���
            MPI_Recv(&myIntegrated, 1, MPI_DOUBLE, source,
                     0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // ��Ƭ����ۼ�
            total += myIntegrated;
        }
    }
    else
    {
        // �������������0���ͼ�����
        MPI_Send(&myIntegrated, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    // �ɽ���0�������
    if (processRank == 0)
    {
        printf(
            "With %d (process) �� %d (partions / process) = %d (partitions),\n",
            processCount, PARTITIONS, processCount * PARTITIONS
            );
        printf("We integrated out Pi is approxy %.22lf.\n", total);
    }

    // �����л���
    MPI_Finalize();

    return 0;
}
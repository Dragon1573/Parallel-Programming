#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define SEC2MS(x) (1000 * x)
#define SEC2US(x) SEC2MS(SEC2MS(x))

// 定义快速类型
typedef unsigned long long int uint64;

// 计算阶乘
const uint64 factorial(uint64 x)
{
    uint64 r = 1;
    while (x > 1)
    {
        r *= x--;
    }
    return r;
}

// 快速幂（位运算实现）
const uint64 ipow(uint64 a, uint64 n)
{
    uint64 ans = 1;
    while (n)
    {
        if (n & 1)
        {
            ans *= a;
        }
        a *= a;
        n >>= (long long)1;
    }
    return ans;
}

int main(int argc, const char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: *.exe <threads> <iterations>\n\n");
        return EXIT_FAILURE;
    }

    const int threads = atoi(argv[1]);
    const int n = atoi(argv[2]);
    if (n > 20)
    {
        fprintf(stderr, "[Error] MathError: Limit exeeded!\n\n");
        return EXIT_FAILURE;
    }

    // 开始计时
    const double start = omp_get_wtime();

    // 并行计算
    double res = 0;
#pragma omp parallel num_threads(threads) reduction(+: res)
    for (uint64 k = 0; k <= n; k++)
    {
        uint64 b = ipow(factorial(k), 4);
        uint64 d = ipow(396, 4 * k);
        uint64 m = factorial(4 * k) * (26390 * k + 1103);
        res += (double)m / (b * d);
    }

    // 计时结束
    const double duration = omp_get_wtime() - start;

    printf("圆周率近似值为：%.20lf\n",
           1.0 / (2 * sqrt(2.0) * res / (99.0 * 99.0)));
    if (duration > 1.0)
    {
        printf("计算耗时：%.3lf sec\n\n", duration);
    }
    else if (SEC2MS(duration) > 1.0)
    {
        printf("计算耗时：%.3lf ms\n\n", SEC2MS(duration));
    }
    else
    {
        printf("计算耗时：%.3lf us\n\n", SEC2US(duration));
    }

    return EXIT_SUCCESS;
}
# 使用`MPI`和`OpenMP`在复平面上绘制曼德博尔德集

## 上机目的

1. 了解动态任务分配模式——工作池/处理器农庄模式。
2. 掌握使用`MPI`和`OpenMP`进行并行程序设计与实现的方法，了解如何选择恰当的并行程序设计API。

## 上机内容与要求

&emsp;&emsp;曼德勃罗德集（Mandelbolt Set）是一种在复平面上组成分形的点的集合，因由曼德勃罗德教授提出而得名。它可以使用复数二次多项式 $z_{n+1}=z_{n}^{2}+c$ 进行迭代来获得。其中， $c$ 是一个复数常量参数。对于每一个 $c$ ，从 $z_0=0$ 开始对 $z$ 进行迭代。𝑧的值或者延伸到无限大，或者只停留在有限半径的圆盘内（这与不同的常量 $c$ 有关）。

&emsp;&emsp;曼德勃罗德集合就是使以上序列不延伸至无限大的所有 $c$ 点的集合。
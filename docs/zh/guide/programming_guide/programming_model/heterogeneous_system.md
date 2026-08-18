# 异构系统<a name="ZH-CN_TOPIC_0000002394061301"></a>

一个基于昇腾处理器的异构系统通常包含CPU与昇腾NPU。其中，CPU及其内存称为Host与Host Memory；NPU及其内存称为Device与Device Memory。

基于昇腾的应用程序通常包含两部分：一部分运行在Host CPU上，使用标准C/C++编程；另一部分运行在NPU上，使用Ascend C编程语言编写。运行在NPU上的代码称为[核函数（Kernel）](./ai_core_simd_programming/kernel_function.md)，需由Host代码调用执行。此外，Host端需要通过CANN Runtime API完成以下操作：在Host Memory与Device Memory之间拷贝数据、启动NPU上的核函数（Kernel）、等待核函数（Kernel）执行完成等。Host代码与核函数（Kernel）可编写在同一个 .asc后缀文件中，由毕昇编译器完成异构编译。

为提升计算效率，NPU上通常有多个计算核并发执行，每个核一般处理不同的数据。每个NPU的计算核称为AI Core（AI处理器的计算核心）。传统上，AI Core遵循SIMD（Single Instruction Multiple Data，单指令多数据流）模型，通过一条指令同时操作多个数据实现并行计算。从Ascend 950PR/Ascend 950DT架构开始，作为SIMD的补充（主要用于辅助离散类矢量算子开发），AI Core也支持SIMT（Single Instruction Multiple Thread，单指令多线程）模型，通过一条指令驱动多个线程并行执行。

下图演示了运行在Host CPU上的代码通过调用CANN Runtime API将核函数（Kernel）下发到AI Core的流程。由于Host Memory与Device Memory拥有独立的内存空间，需要显式调用Runtime API进行数据传输。典型流程为：Host代码先将输入数据从Host Memory传输到Device Memory，随后启动Device侧核函数（Kernel）并等待其执行完成，最后将计算结果从Device Memory拷贝回Host Memory。


**图1**  核函数（Kernel）调度示意图<a name="fig573916102285"></a><br>
<img src="../../figures/kernel_sched_2.png" alt="核函数（Kernel）调度示意图" width="520px">


>[!NOTE]说明
> 关于Task/Stream等Runtime运行时管理的详细介绍，请参考[《Runtime运行时API》](https://hiascend.com/document/redirect/CannCommunityRuntimeApi)。

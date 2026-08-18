# 抽象硬件架构<a name="ZH-CN_TOPIC_0000002556525479"></a>

AI Core上的SIMT（Single Instruction Multiple Thread，单指令多线程）编程允许指令对数据进行独立寻址，从而支持线程级并行计算。这种方式特别适用于离散访问和复杂控制逻辑等场景<!-- npu="950" id1 -->，目前仅支持Ascend 950PR/Ascend 950DT<!-- end id1 -->。SIMT编程能够有效简化复杂算子和不规则控制流的开发，缓解包含分支的发散计算并控制程序复杂度，从而提高硬件利用率和能效。

如[图1](#fig35720259287)所示，AI处理器内部有多个Vector Core，每个Vector Core包含计算单元、Shared Memory（位于Unified Buffer（UB））、寄存器，核外的Global Memory是全局内存空间，被所有Vector Core共享。

**图1**  SIMT抽象硬件架构图<a name="fig35720259287"></a>  
![](../../../figures/simt_hw.png "SIMT抽象硬件架构图")

以下是SIMT多线程计算涉及到的硬件资源的说明：

-   每个线程拥有独立的寄存器，用于存储局部数据，寄存器的数量受线程块内线程数量影响，线程数量越多，每个线程拥有的寄存器数量越少。
-   UB按功能划分为多个区域，其中一部分内存空间作为线程块内所有线程的共享内存（Shared Memory），支持线程块内的线程进行数据交互；一部分作为Data Cache，用于缓存全局内存数据。
-   L2 Cache是所有Vector Core共享的高速缓存，位于Global Memory与各Vector Core的Data Cache之间，用于缓存全局内存数据以减少访问延迟。L2 Cache由硬件自动管理，用户无需显式配置。
-   SIMT模式中，读取Global Memory上的数据时，通过Data Cache单元完成数据中转，数据流经由Global Memory到Data Cache，再从Data Cache到寄存器。Data Cache是UB中预留的一部分空间，实际大小由用户自主分配。

若您对上述内容中的线程、线程块等概念不熟悉，建议查阅[线程架构](thread_architecture.md)了解更多SIMT线程架构知识；同时，您也可以通过阅读[内存层级](memory_hierarchy.md)，了解如何在UB中配置共享内存与Data Cache的空间大小。

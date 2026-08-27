# 编程模型

## 适用场景

SIMT编程适合控制流复杂和访存模式不规则的算子场景。由于每个线程拥有独立的控制流和地址计算能力，SIMT能够自然地处理条件分支逻辑，随机访问内存而无需受限于连续访存模式。同时，SIMT提供原子操作和线程间内存屏障等机制，支撑多线程对同一片内存的安全并发访问，适用于哈希表等需要冲突解决与数据同步的数据结构操作。SIMT的优势在于以线程为最小调度单元赋予每条执行路径充分的独立性，从而在控制流复杂和访存模式不规则的场景下能够获得更高的开发效率与执行性能。

## 线程架构

SIMT编程模型采用分层的抽象线程组织结构，示意图如下所示，从顶层到底层依次为：Grid（线程块网格）、Thread Block（线程块）、Warp（线程束）和Thread（线程）。这种层次化的设计使得开发者可以方便地将并行计算问题映射到硬件资源上。

**图1**  线程结构示意图

![](../../figures/thread_structure.png "线程结构示意图")

**Thread（线程）**

线程是整个结构中的最小单元，每个线程独立完成计算任务，拥有独立的寄存器和栈空间。

**Thread Block（线程块）**

Thread Block是Grid的组成单元，由若干线程组成（最大2048个线程）。使用内置变量[blockDim](extended_syntax/builtin_variables.md#blockdim)来表示一个Thread Block启用的线程数量。在SIMT编程场景中，Thread Block有以下特点：

-   同一Thread Block内的线程可以访问共享内存，即Unified Buffer（UB），以实现数据交互；Thread Block内的线程可通过同步机制实现协作；
-   在SIMT函数定义时可使用[\_\_launch\_bounds\_\_\(\)](extended_syntax/kernel_function_config.md#__launch_bounds__n)配置最大线程数。

**Grid（线程块网格）**

Grid是SIMT线程层次结构的最顶层，由多个Thread Block组成。使用内置变量[gridDim](extended_syntax/builtin_variables.md#griddim)来表示Grid中启用的线程块数量。在SIMT编程场景，Grid有以下特征：

-   Grid的维度配置由用户设置启动的AIV核数决定，核函数（Kernel）执行期间不可更改；
-   Grid中的所有线程块具有相同的尺寸和维度配置；
-   同一Grid中的线程块相互独立，按任意顺序执行；

限制说明如下：

-   Grid中线程块总数不能超过65535。

**线程索引**

每个线程都有唯一的标识，开发者可以通过内置变量获取线程的信息，从而确定每个线程负责处理的数据，相关内置变量如下表所示：

| 内置变量 | 说明 | 数据类型 | 约束 |
| --- | --- | --- | --- |
| gridDim | Grid的维度大小 | dim3 | gridDim.x * gridDim.y * gridDim.z <= 65535 |
| blockDim | Thread Block的维度大小 | dim3 | blockDim.x * blockDim.y * blockDim.z <= 2048 |
| blockIdx | 当前Thread Block在Grid中的索引 | dim3 | 无 |
| threadIdx | 当前线程在Thread Block内的索引 | dim3 | 无 |

各Thread Block可通过线程块索引blockIdx进行标识，各线程可通过线程块内线程索引threadIdx进行标识。对于一维Grid和Block，索引计算公式为：

```cpp
int idx = blockIdx.x * blockDim.x + threadIdx.x;
```

对于二维Grid和Block，索引计算公式为：

```cpp
int x_index = blockIdx.x * blockDim.x + threadIdx.x;
int y_index = blockIdx.y * blockDim.y + threadIdx.y;
```

## Warp执行机制

Warp是SIMT架构中基本的调度和执行单位。每个Warp包含32个线程，这些线程从相同的程序地址开始执行，拥有各自的指令地址计数器和寄存器状态，并且可以选择分支独立执行。在一个Thread Block内，所有线程按线性顺序被硬件自动划分为每32个线程一组的Warp，同一Warp内的所有线程执行同一条指令。

Warp内的线程虽然执行同一段代码，但可通过条件分支进入不同的执行路径，这种情况称为分支发散（Warp Divergence）。当Warp中的32个线程均执行相同的代码分支时，硬件利用率最高；一旦发生分支发散，硬件会串行执行每个分支路径，只有进入当前分支的线程（即活跃线程）会被执行，其余线程则被屏蔽，从而导致Warp执行效率下降。此外，同一Warp内的线程相互独立，彼此不能存在依赖关系。

Thread Block的线程数建议设置为32的整数倍。若线程数未满足该要求，最后一个Warp将包含不足32个线程，导致该Warp内存在空闲线程通道，从而降低执行效率。

## UB划分

UB是同一线程块内所有线程均可访问的内存空间，位于每个AIV内部。UB内存空间总大小为256KB，参考[图2](#fig9500834165913)，按功能划分为四个主要区域，从低地址向高地址依次为静态内存、动态内存、预留空间、Data Cache。

**图2**  UB内存分配图<a name="fig9500834165913"></a>  
![](../../figures/ub_memory_allocation.png "UB内存分配图")

### UB内存空间说明

1.  静态内存：从内存的起始地址分配一段指定大小的内存空间，其大小在编译时确定，不可动态修改。

    ```cpp
    // 静态内存通过数组分配
    __ubuf__ char static_buf[1024];
    ```

2.  动态内存：位于静态内存之后，通过[<<<...\>\>\>](extended_syntax/kernel_function_config.md#调用)中参数dyn\_ubuf\_size指定大小的动态内存空间，可通过以下方式申请使用：

    ```cpp
    // 通过动态数组申请使用动态内存
    extern __ubuf__ char dynamic_buf[];
    ```

3.  预留空间：系统预留空间，大小固定为8KB。
4.  Data Cache：SIMT专有的Data Cache空间，UB内扣除静态内存、动态内存以及预留空间以后，剩余内存大小为Data Cache，Data Cache最小为32KB，剩余空间超过128KB时Data Cache大小固定为128KB，具体计算公式为：

    ```
    DataCache = min(UB总大小（256KB） - 静态内存 - 动态内存 - 预留空间(8KB), 128KB)
    ```

    若DataCache小于32KB，会出现校验报错。

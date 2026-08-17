# AI Core SIMT编程模型概述

## 引言

SIMT（Single Instruction Multiple Thread，单指令多线程）编程是AI Core编程方式的一种重要补充，特别适用于离散数据访问、复杂控制逻辑和分支发散等场景。Ascend C支持与业界一致的SIMT编程模型，通过线程级并行实现数据并行计算，每个线程允许指令对数据进行独立寻址与计算，从而实现了更高的编程灵活性。本章将以**SIMT编程模型**为主线，遵循「宏观架构—线程调度—代码落地」的递进逻辑，完整覆盖SIMT算子开发全链路：阐述多线程并行架构的执行机制，拆解线程块内Warp调度的底层逻辑，并完成算子Kernel计算逻辑的开发。

## 异构并行计算核心模型：SIMT编程范式

SIMT编程支持业界通用的线程架构：**Grid-Block-Thread**，以线程作为最小执行单元，通过大规模线程并发实现数据并行计算。所有SIMT算子的并行逻辑、数据拆分策略、线程调度机制，均基于该编程范式实现。

**三级线程层级**：
- **网格（Grid）**：由多个线程块（Thread Block）组成，使用内置变量[gridDim](../../language_extension/simt_builtin_keywords.md#li20760123812911)表示启用的线程块个数，Grid维度通过dim3三维结构表示。
- **线程块（Thread Block）**：由若干线程组成，使用内置变量[blockDim](../../language_extension/simt_builtin_keywords.md#li076017381191)表示一个线程块启用的线程个数（一个线程块最多可启用2048个线程）。各线程块通过线程块索引[blockIdx](../../language_extension/simt_builtin_keywords.md#li1676053814914)进行标识。
- **线程（Thread）**：最小执行单位，每个线程拥有独立的寄存器，可通过线程索引[threadIdx](../../language_extension/simt_builtin_keywords.md#li7760123814919)标识，负责完成特定的数据计算任务。

**Warp调度机制**：

每个线程块被切分成多个Warp依次调度执行，Warp是执行相同指令的线程集合，每个Warp包含32个线程。每个AIV核包含多个Warp调度器（Warp Scheduler），当某个Warp因全局内存访问处于阻塞状态时，Warp调度器会立刻切换到就绪的Warp，确保计算单元处于忙碌状态，从而掩盖访存的延迟并实现硬件利用率最大化。

> 📌 **提示**：基于SIMT编程模型的程序，在AIV核上执行多个结构相同的线程块，执行的总线程数等于gridDim × blockDim。每个线程指向相同的代码逻辑，但处理不同的数据片段，实现数据并行。

## 硬件底层支撑：SIMT单元架构与内存层级

向量处理单元[AIV核](../../advanced_programming/hardware_implementation/basic_architecture.md)是SIMT编程的核心硬件载体，采用「Warp调度器 + 计算单元 + 分级存储体系」的架构，下文将详细介绍SIMT核心硬件组件及SIMT编程的内存层级体系。

### 核心硬件组件

- **Warp调度器**：每个AIV核包含多个Warp调度器，负责线程块的切分与调度。Warp调度器将线程块划分为多个Warp，依次发射执行，实现高效的线程级并行调度。
- **计算单元**：AIV核内的向量计算单元，负责执行各类向量运算指令。同一Warp内的32个线程执行相同指令，但对不同数据地址进行操作，实现单指令多线程并行计算。
- **寄存器文件**：每个线程拥有独立的寄存器空间，用于存储局部变量和计算中间结果。寄存器数量受线程块内线程数量影响，线程数量越多，每个线程可用的寄存器数量越少。
- **共享内存**：线程块内所有线程共享的内存空间，位于AIV核内部，具有高带宽、低延迟特性，可作为用户管理的高速缓存，支持线程间数据交互。


### SIMT编程内存层级体系

SIMT线程可访问多种内存空间，形成「全局内存-共享内存-寄存器」三级内存层级，各层级作用域、生命周期与物理位置差异如下：

| 内存类型 | 线程作用域 | 内存修饰符 | 生命周期 | 物理位置 | 特点 |
|----------|-----------|---------|----------|---------|------|
| **全局内存** | Grid | \_\_gm__ | 应用程序 | Device | 所有线程可直接访问，持久化存储 |
| **共享内存（Unified Buffer）** | Block | \_\_ubuf__ | 核函数 | AIV核 | 线程块内共享，高带宽低延迟 |
| **寄存器** | Thread | NA | 核函数 | AIV核 | 线程私有，编译器管理，数量受blockDim影响 |


## SIMT编程接口体系

### SIMT编程接口特点

SIMT编程接口遵循业界统一方式，开发者通过Ascend C提供的操作符和接口操作全局内存与共享内存，实现对硬件能力的完全控制，同时SIMT编程也提供部分高阶特性用于特定的应用场景，具体接口可以查询[SIMT-API](../../../../api/SIMT-API/SIMT-API.md)。

SIMT编程接口核心特点：
- **内置关键字扩展**：遵循业界统一方式，提供`__global__`修饰符以及`blockIdx`、`threadIdx`、`blockDim`、`gridDim`等线程索引变量。
- **共享内存管理**：支持与业界一致的使用方式，通过`__ubuf__`修饰符申请静态共享内存，支持动态内存申请方式。
- **同步接口支持**：支持与业界一致的同步方式，提供`asc_syncthreads`、`asc_threadfence`等同步接口，确保线程间数据访问正确性。
- **Warp类接口支持**：支持与业界一致的Warp类接口，提供`asc_shfl`、`asc_reduce_max`、`asc_all`等Warp类操作接口，用于直接操作线程束。
- **原子操作支持**：支持与业界一致的使用方式，提供`asc_atomic_add`、`asc_atomic_sub`、`asc_atomic_exch`等原子操作接口。
- **数学函数支持**：遵循业界统一方式，提供`sin`、`cos`、`exp`、`__brev`、`__clz`等计算接口，支持`half`、`half2`、`bfloat16_t`、`bfloat16x2_t`、`float`、`float2`等多种数据类型。
- **高阶特性支持**：当前提供协作组特性，用于需要灵活控制分组的业务场景。

## 小结

本章系统阐述了SIMT算子从线程架构、硬件调度到代码落地的全链路技术体系：计算任务基于Grid-Block-Thread三级线程层级分发至AIV核并行执行，通过线程索引自动映射数据范围，实现大规模线程并发计算。开发者基于SIMT语言编程接口，通过指针操作全局内存与共享内存，配合同步机制确保线程间数据访问正确性。

后续章节将深入详解[抽象硬件架构](./abstract_hardware_architecture.md)、[线程架构](./thread_architecture.md)、[核函数](./kernel_function.md)定义与调用、[内存层级](./memory_hierarchy.md)管理机制、[同步机制](./synchronization.md)原理、[原子操作](./atomic_operations.md)使用方式、[编程示例](./programming_examples.md)实战流程，帮助开发者快速理解核心技术，熟练掌握高性能、高可用的SIMT算子开发技术。

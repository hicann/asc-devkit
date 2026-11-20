# AI Core SIMD Programming Model Overview

<!-- md-trans-meta sourceCommit=5c07153f668df7dd8585d7a94d76b4114ca4b1ac translatedAt=2026-11-09T11:09:04.099Z pushedAt=2026-11-19T06:32:19.979Z nl=2 -->

## Introduction

The SIMD parallel mechanism is the core computing power of AI Core, accounting for more than 90% of the total computing throughput of the entire device. Therefore, this chapter takes the **SIMD programming model** as the main thread, follows the progressive logic of "macro architecture - hardware scheduling - code implementation", and fully covers the entire operator development chain of SIMD: it describes the parallel task distribution mechanism of multi-AI Core clusters, breaks down the task scheduling logic of heterogeneous compute units within a single AI Core, and explains layer by layer how to develop the computation logic of the [operator kernel function](./kernel_function.md) based on differentiated C/C++ programming APIs.

## Core Model of Heterogeneous Parallel Computing: SPMD Nested SIMD Programming Paradigm

The Ascend NPU heterogeneous parallel system adopts a two-layer fused architecture of **outer multi-core SPMD (Single Program Multiple Data) parallelism \+ inner single-core SIMD fine-grained parallelism**, which is the core theoretical foundation of AI Core operator development. The parallel logic, data splitting strategy, and task scheduling mechanism of all SIMD operators are implemented based on this programming paradigm.

**The multi-core cluster level follows the SPMD programming model**: each AI Core is abstracted as a Block, with the built-in variable [block_idx](../../language_extension/simd_builtin_keywords.md) serving as the Block index. Each Block executes the same operator kernel function code, and generally divides the data processing range of each Block based on [block_idx](../../language_extension/simd_builtin_keywords.md) to achieve multi-core load balancing and parallel scheduling.

**The single-core computation level relies on the SIMD parallel mechanism**: within a single AI Core, fine-grained parallelism is implemented based on the [SIMD](../programming_model_overview.md) mechanism. Relying on dedicated hardware compute units, a single instruction completes parallel operations on multiple groups of homogeneous data in batches, fully tapping the ultimate computing potential of a single AI Core.

## Hardware Foundation: AI Core Compute Unit Architecture and Operator Classification

The AI Core is the fundamental core compute unit of the Ascend NPU. It adopts a classic architecture of "control unit \+ heterogeneous compute unit \+ hierarchical local storage", in which each component has a clear role and works in coordination. The hardware capability boundary directly determines the computation form and development approach of operators. The following sections describe the core hardware components of the AI Core in detail, as well as the standardized operator types classified based on hardware capabilities.

### AI Core Key Hardware Components

- **Scalar Processing Unit**: As the control center of the AI Core, it is mainly responsible for address offset compute, instruction scheduling and dispatch, coordinating and managing the instruction execution flow of other compute units, and supporting the normal operation of various control flow logic such as branches and loops.
- **Vector Processing Unit [Vector](../../advanced_programming/hardware_implementation/basic_architecture.md)**: Following the standard SIMD parallel computation logic, it is dedicated to executing various vector instructions, supports single-instruction multiple-data parallel operations, and adapts to computation scenarios with high flexibility requirements such as element-wise computation, logical operations, and data reorganization.
- **Matrix Computation Unit [Cube](../../advanced_programming/hardware_implementation/basic_architecture.md)**: A high-density tensor-dedicated compute unit, deeply optimized for compute-intensive scenarios such as cube multiply-accumulate and high-dimensional tensor convolution. The hardware natively supports batch cube operations, with the typical capability of completing a group of float16 16×16 cube multiplications in a single operation, serving as the core compute support for AI model training and inference scenarios.

- **Local Memory**: The on-chip high-speed storage system of the AI Core, used to cache intermediate computation data, avoid the performance loss of frequently accessing low-speed global memory, and reduce memory access latency. The Cube compute unit is paired with [L1 Buffer](../../advanced_programming/hardware_implementation/basic_architecture.md), [L0C Buffer](../../advanced_programming/hardware_implementation/basic_architecture.md), and so on; the Vector compute unit is paired with the unified cache [Unified Buffer (UB)](../../advanced_programming/hardware_implementation/basic_architecture.md).<!-- npu="950" id1 -->The Vector unit of the AI Core in Ascend 950PR&950DT products adds programmable vector registers (Register), with a single register size of 256B.<!-- end id1 -->


### Three Standard Operator Types Based on Hardware Units

Based on the differences in how an operator's core computation logic depends on hardware units, AI Core operators can be divided into three standard types. Dedicated modifiers standardize hardware resource scheduling rules and unify the operator development specification across all domains:
- **Vector operators**: The core computation logic is entirely carried by the Vector compute unit, with no cube-intensive operations. They focus on element-wise, logical, and data-reorganization computations, and are suitable for memory-access-intensive scenarios that require high flexibility. The kernel function is modified by [\_\_vector\_\_](../../language_extension/simd_builtin_keywords.md).
- **Matrix operators**: The core computation logic is entirely carried by the Cube compute unit, focusing on regular compute-intensive operations such as large-dimension cube multiplication and tensor convolution to pursue ultimate hardware throughput. The kernel function is modified by [\_\_cube\_\_](../../language_extension/simd_builtin_keywords.md).
- **Fusion operators**: The mainstream complex operator form in the industry. They can coordinate the Cube compute unit and the Vector compute unit for collaborative computation, combining high-density cube computing power with flexible vector logic processing. The kernel function is modified by [\_\_mix\_\_(cube, vec)](../../language_extension/simd_builtin_keywords.md), where \_\_mix\_\_(cube, vec) indicates the ratio of launched Cube cores to Vector cores. The supported ratios include (1, 0), (0, 1), (1, 1), and (1, 2).

> 📌 **Note**: The NPU hardware architecture can be divided into [decoupled mode](../../advanced_programming/hardware_implementation/basic_architecture.md) and [coupling mode](../../advanced_programming/hardware_implementation/basic_architecture.md). In decoupled mode, the two types of compute units each have an independent scalar control unit. In coupling mode, the Cube and Vector units share the same scalar control unit. The \_\_mix\_\_ modifier takes effect only in decoupled mode; the coupling-mode hardware does not support this feature. For more detailed hardware architecture information, see the [Hardware Implementation](../../advanced_programming/hardware_implementation/basic_architecture.md) section.

## General Steps for SIMD Operator Development Based on AI Core

Based on the SPMD\+SIMD two-layer programming model, all AI Core cores run the same kernel function and distinguish data slices through **[block_idx](../../language_extension/simd_builtin_keywords.md)** to implement multi-core parallel computation. Therefore, the first step of operator development is **Tiling design**: evenly partition the ultra-large global tensor data, assign an independent data block to each AI Core, and ensure load balancing across multiple cores.

In addition, unlike traditional CPU serial programming and [SIMT](../programming_model_overview.md) thread programming, a distinctive feature of AI Core SIMD programming is **explicit hierarchical memory access**: developers must manually manage data flow, moving data from Global Memory (also known as Device Memory, abbreviated as GM) to the on-chip Local Memory of the AI Core (typically including L1 Buffer and UB) for computation, and finally writing the computation results back to global memory.

### General Four-Step Method for Operator Kernel Function Development

SIMD-based AI Core operator development consists of four main steps, which apply to most operator development scenarios:
1. **Tiling design**: Evenly partition the global ultra-large tensor data, allocate balanced independent data tiles to each AI Core, precisely adapt to the SPMD multi-core parallel architecture, avoid single-core compute bottlenecks, and achieve full-domain load balancing.
2. **Data transfer in**: Call the SIMD programming API to move the data to be computed from global memory in batches into high-speed on-chip local memory of the AI Core, such as L1 Buffer and UB.
3. **Data computation**: Schedule the Cube compute unit or the Vector compute unit based on the operator type, and complete cube, vector, or mixed logic computation using the on-chip cached data. For the same computation task, subsequent sections will introduce multi-level differentiated programming APIs that can respectively adapt to different development requirements such as development efficiency, ultimate performance, and C/C\+\+ programming practice.
4. **Data transfer out**: After computation is complete, write the result data in on-chip storage back to global memory to complete a single operator computation process.

> 📌 **Note**: The on-chip local storage of the AI Core is limited and cannot load ultra-large tensors at once. In actual development, the strategy of "iterative tiled transfer, batched computation, and result accumulation" is commonly adopted to complete full-domain data computation, while pipeline techniques are used to hide the data movement latency and improve overall computation efficiency.

### Dual-Mode Vector Computation Structure of the New Architecture

The new-generation architecture of the Ascend 950PR&950DT products opens up register programmability on top of the traditional UB cache system, building a three-level memory hierarchy of "Global Memory → UB → Register". This gives rise to two vector computation modes adapted to different performance requirements, achieving full coverage of both general-purpose scenarios and ultimate-performance scenarios. The memory hierarchy is shown in the following figure:

**Figure 1**  SIMD Reg vector computation memory hierarchy  
<img src="../../../figures/simd_reg_2.png" alt="SIMD-Reg vector computation memory hierarchy" width="520px">

Based on the new three-level memory architecture, vector computation is divided into the general-purpose **Memory vector computation** and the high-performance **Reg vector computation**:

- **Memory vector computation (traditional general-purpose mode)**: It features full-architecture compatibility, a simple and stable process, and strong versatility, performing data caching and computation entirely based on UB.
  - Data transfer in: Global Memory → UB
  - Vector computation: Perform vector computation based on **UB**
  - Data transfer out: UB → Global Memory

- **Reg vector computation (high-performance mode)**: Leveraging the low-latency and high-bandwidth hardware advantages of registers, it is designed specifically for ultimate-performance optimization scenarios:
  - Data transfer in: Global Memory → UB → Register (**Reg**)
  - Vector computation: Perform vector computation based on **Reg**.
  - Data transfer out: Registers (**Reg**) → UB → global memory

Vector registers reside in the innermost layer of the Vector compute unit, offering the lowest memory access latency and the highest bandwidth. Exposing register programmability helps developers fully unleash the peak computing performance of the hardware.

## Multi-Level Programming API System: Layered Abstraction and Capability Differences

### Review of Multi-Level Operator Programming API Evolution

Early operator development centered on raw-pointer programming in C. Leveraging the low-level memory manipulation capabilities of C, developers could directly address and operate device memory through pointers, while native APIs that map precisely to low-level hardware instructions supported the invocation of hardware primitives such as vector computation, cube multiplication, and data movement. This model enables fine-grained control over hardware resources such as caches and registers, serving as the core solution for low-level ultimate performance optimization. It is widely used in the low-level implementation of core operators such as discrete, reduction, and cube operators.

As deep learning continues to evolve, the core carrier of AI computation has been upgraded to 4D/5D high-dimensional tensors (such as attention tensors in NLP models and feature map tensors in CV models). Traditional raw-pointer programming requires manually performing repetitive tasks such as memory offset compute, dimension splitting, and boundary checking, which not only results in low development efficiency but also easily introduces stability issues such as out-of-bounds memory access and indexing errors.

Against this background, the Tensor abstraction based on C++ object-oriented features emerged. A Tensor embeds tensor metadata such as memory layout and data type, where the layout includes dimensions (shape) and strides, simplifying memory layout management and reducing the development cost of high-dimensional tensors. The industry has thus gradually formed the mainstream Tensor/Tile programming model: the core idea is to partition a global tensor into regular, fixed-size data blocks (tiles), with the tile serving as the smallest unit of computation, storage, and hardware scheduling. Among them, Tensor is a standardized mathematical and computational abstraction of multidimensional arrays, serving as the unified representation of various data types such as vectors, cubes, and high-dimensional feature maps in AI scenarios. It describes only the global logical structure and is not bound to hardware execution details. Tile, in contrast, is the smallest scheduling, computation, and data movement granularity of the tensor cores and vector cores of AI acceleration chips. As the physical-layer carrier of the programming model, it bridges global data and underlying hardware. By partitioning a global Tensor into tiles that match the hardware storage and compute specifications, and combining optimization techniques such as data movement, pipeline scheduling, and on-chip memory reuse, the ultimate hardware computing power can be fully exploited and unleashed.

The current Tensor/Tile programming paradigm mainly includes two implementation paths: manual Tensor/Tile programming is generally implemented based on C/C\+\+, with developers fully controlling the entire process of memory layout, tiling strategy, data movement, and pipeline scheduling; automated Tensor/Tile programming is generally implemented based on Python DSLs (domain-specific languages), such as [PyPTO](https://gitcode.com/cann/pypto), [TileLang](https://github.com/tile-ai/tilelang-ascend), and [Triton](https://github.com/openai/triton), where the AI compiler automatically handles memory allocation, resource reuse, task scheduling, and dependency synchronization, lowering the development barrier.

### Ascend C Multi-Level Programming APIs

Adhering to the core design principle of "standard C/C\+\+ syntax with minimal extensions", Ascend C builds a lightweight, high-performance low-level programming foundation that provides ultimate-performance-oriented operator development capabilities for Ascend series chips. It also precisely adapts to the differentiated programming practices of C and C\+\+ developers, achieving an optimal balance between fine-grained low-level hardware control and efficient upper-layer engineering development.

Based on the above design philosophy and common industry development practices, Ascend C natively provides two core programming systems: a C language API for pointer-based programming, and a C++ class library basic API for Tensor-based programming. In addition, drawing on the classic C/C++ queue pipeline design, Ascend C innovatively introduces the TPipe/TQue programming framework, which simplifies the complexity of memory scheduling, data synchronization, and pipeline control in heterogeneous computing scenarios, effectively lowering the barrier to developing and deploying high-performance operators.

Ascend C provides three tiers of gradient programmable APIs. All tiers support complete AI Core compute scheduling, with progressive capabilities and clear scenario adaptation at each tier. The specific differences are as follows:

| API Tier | Language | Characteristics | Primary Use |
|----------|------|------|----------|
| **TPipe/TQue framework programming API** | **C++** | Based on **Tensor** programming, automatically manages memory movement and synchronization through **TPipe/TQue**, shielding underlying details. | Uses the framework to automatically orchestrate data movement and computation, improving programming ease of use and development efficiency. |
| **Basic API** | **C++** | Based on **Tensor** programming, provides **complete C++ programming capabilities**, allocates Tensors through `LocalMemoryAllocator` and other mechanisms, with synchronization self-managed by the developer. | Self-manages synchronization and memory layout, adapts to C\+\+ Tensor development practices, exposes all underlying hardware capabilities, and supports fine-grained tuning and ultimate performance. |
| **Language extension layer SIMD API** | **C** | Based on **pointer** programming, provides **complete C programming capabilities**, manages local memory by declaring static arrays, with synchronization self-managed by the developer. | Self-manages synchronization and memory, adapts to C language development practices, exposes all underlying hardware capabilities, and supports fine-grained tuning and ultimate performance. |

Developers can flexibly select the appropriate tier of APIs to complete operator development based on their own programming practices, project development efficiency requirements, and performance optimization scenarios.

> 📌 **Note** Ascend C uses GlobalTensor and LocalTensor to abstract the Tensor/Tile corresponding to global memory and on-chip local memory, respectively. In the current version, LocalTensor does not carry Layout memory layout information. In future versions, LocalTensor in the Tensor API will natively support Layout management. For the specific release schedule, see the [Ascend C DevKit repository](https://gitcode.com/cann/asc-devkit).

## Summary

This chapter systematically reviews the end-to-end technical system of AI Core operator development, from task distribution and hardware scheduling to code implementation: compute tasks are distributed to multiple AI Cores for parallel execution based on the SPMD\+SIMD heterogeneous parallelism paradigm, and are then precisely scheduled to the Cube unit or the Vector unit according to the computational characteristics of the operator. Developers can flexibly select multi-level C/C++ programming APIs based on their own programming practices and business scenarios to implement the complete logic of the operator kernel function.

Subsequent chapters will progressively explain, layer by layer, the hardware architecture principles, kernel function definition, specifications of programming APIs at each level, memory management mechanisms, data synchronization logic, hands-on development process, and performance tuning techniques, helping developers quickly understand the core technologies and master high-performance, highly available AI Core operator development capabilities.

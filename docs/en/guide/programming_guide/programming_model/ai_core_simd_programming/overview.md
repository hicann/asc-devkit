# AI Core SIMD Programming Model Overview

<!-- md-trans-meta sourceCommit=198cbfdf29f72f78c8de735d88a5244dc258f2e7 translatedAt=2026-08-20T11:01:18.866Z pushedAt=2026-08-26T03:06:49.567Z -->

## Overview

The SIMD parallel mechanism is the core compute power of the AI Core, accounting for more than 90% of the total compute throughput of the entire system. Therefore, this chapter takes the **SIMD programming model** as its main thread and follows the progressive logic of "macro architecture - hardware scheduling - code implementation" to fully cover the entire SIMD operator development chain: it explains the parallel task distribution mechanism of multi-AI Core clusters, breaks down the task scheduling logic of heterogeneous compute units within a single AI Core, and explains layer by layer how to complete the development of the [operator kernel](./kernel_function.md) computation logic based on differentiated C/C++ programming APIs.

## Heterogeneous Parallel Compute Core Model: SPMD Nested SIMD Programming Paradigm

The Ascend NPU heterogeneous parallel system adopts a two-layer fused architecture of **outer multi-core SPMD (Single Program Multiple Data) parallelism + inner single-core SIMD fine-grained parallelism**, which serves as the core theoretical foundation for AI Core operator development. The parallel logic, data splitting strategy, and task scheduling mechanism of all SIMD operators are implemented based on this programming paradigm.

**The multi-core cluster level follows the SPMD programming model**: each AI Core is abstracted as a Block, and the built-in variable [block_idx](../../language_extension/SIMD-BuiltIn_keyword.md) is used as the Block index. Each Block executes the same operator kernel function code, and generally divides the data processing range of each Block based on [block_idx](../../language_extension/SIMD-BuiltIn_keyword.md), thereby achieving multi-core load balancing and parallel scheduling.

**The single-core computation level relies on the SIMD parallel mechanism**: within a single AI Core, fine-grained parallelism is implemented based on the [SIMD](../programming_model_overview.md) mechanism. By leveraging dedicated hardware compute units, a single instruction completes parallel operations on multiple groups of homogeneous data in batches, fully tapping the ultimate compute potential of a single AI Core.

## Hardware Foundation: AI Core Compute Unit Architecture and Operator Classification

The AI Core is the fundamental core compute unit of the Ascend NPU. It adopts a classic architecture of "control unit \+ heterogeneous compute units \+ hierarchical local storage", in which each component has a clear role and works in coordination. The capability boundary of the hardware directly determines the computation form and development approach of operators. The following sections describe the core hardware components of the AI Core in detail, as well as the standardized operator types classified based on hardware capabilities.

### AI Core Hardware Components

- **Scalar processing unit**: As the control center of the AI Core, it is mainly responsible for address offset computation, instruction scheduling and dispatch, coordinating and managing the instruction execution flow of other compute units, and supporting the normal operation of various control flow logic such as branching and looping.

- **[Vector](../../advanced_programming/hardware_implementation/basic_architecture.md) processing unit**: Following the standard SIMD parallel computation logic, it is dedicated to executing various vector instructions, supports single-instruction multiple-data parallel operations, and adapts to computation scenarios with high flexibility requirements such as element-wise computation, logical operations, and data reorganization.

- **[Cube](../../advanced_programming/hardware_implementation/basic_architecture.md) compute unit**: A high-density tensor-specific compute unit, deeply optimized for compute-intensive scenarios such as cube multiply-accumulate and high-dimensional tensor convolution. The hardware natively supports batched cube operations, with a typical capability of completing a group of float16 16×16 cube multiplications in a single operation, serving as the core computation support for AI model training and inference scenarios.

- **Local storage**: On-chip high-speed storage system of the AI Core, used to cache intermediate computation data, avoid the performance loss of frequently accessing low-speed global memory, and reduce memory access latency. The Cube compute unit is paired with [L1 Buffer](../../advanced_programming/hardware_implementation/basic_architecture.md), [L0C Buffer](../../advanced_programming/hardware_implementation/basic_architecture.md), and so on; the Vector compute unit is paired with the unified buffer [UB](../../advanced_programming/hardware_implementation/basic_architecture.md); the AI Core vector unit of Ascend 950PR/Ascend 950DT adds programmable vector registers, with a single register size of 256 bytes.

### Three Standard Operator Types Based on Hardware Units

Based on the differences in how an operator's core computation logic depends on hardware units, AI Core operators are classified into three standard types. Dedicated modifiers standardize the hardware resource scheduling rules and unify the operator development conventions across the global domain:

- **Vector operators**: The core computation logic is entirely carried by the Vector compute unit, with no cube-intensive operations. They focus on element-wise, logical, and data-reorganization computations, and are suited to memory-access-intensive scenarios that require high flexibility. The kernel function is modified by [\_\_vector\_\_](../../language_extension/SIMD-BuiltIn_keyword.md).

- **Cube operators**: The core computation logic is entirely carried by the Cube compute unit, focusing on regular compute-intensive operations such as large-dimension cube multiplication and tensor convolution to pursue ultimate hardware throughput. The kernel function is modified by [\_\_cube\_\_](../../language_extension/SIMD-BuiltIn_keyword.md).

- **Fusion operators**: Mainstream complex operator form in the industry. They can jointly schedule the Cube compute unit and the Vector compute unit for collaborative computation, combining high-density matrix compute power with flexible vector logic processing. The kernel function is modified by [\_\_mix\_\_(cube, vec)](../../language_extension/SIMD-BuiltIn_keyword.md), where \_\_mix\_\_(cube, vec) indicates the ratio of Cube cores to Vector cores to be launched. The supported ratios include (1, 0), (0, 1), (1, 1), and (1, 2).

> 📌 **Note:** The NPU hardware architecture can be divided into [decoupled mode](../../advanced_programming/hardware_implementation/basic_architecture.md) and [coupled mode](../../advanced_programming/hardware_implementation/basic_architecture.md). In decoupled mode, the two types of compute units each have an independent scalar control unit. In coupled mode, the Cube and Vector units share the same scalar control unit. The `__mix__` modifier takes effect only in decoupled mode, and the coupled mode hardware does not support this feature. For more detailed hardware architecture information, see the [Hardware Implementation](../../advanced_programming/hardware_implementation/basic_architecture.md) section.

## General Steps for AI Core-Based SIMD Operator Development

Based on the SPMD\+SIMD two-layer programming model, all AI Cores run the same kernel function and use **[block_idx](../../language_extension/SIMD-BuiltIn_keyword.md)** to distinguish data shards, thereby implementing multi-core parallel computation. Therefore, the primary step of operator development is **Tiling design**: evenly partition the global ultra-large tensor data, allocate an independent data block to each AI Core, and ensure load balancing across multiple cores.

In addition, unlike traditional CPU serial programming and [SIMT](../programming_model_overview.md) thread programming, a distinctive feature of AI Core SIMD programming is **explicit hierarchical memory access**: developers must manually manage data flow, move data from Global Memory (also called Device Memory, abbreviated as GM) to the on-chip Local Memory of the AI Core (typically including L1 Buffer and UB) for computation, and finally write the computation results back to Global Memory.

### General Four-Step Operator Kernel Development Process

SIMD-based AI Core operator development consists of four main steps, which apply to most operator development scenarios:

1. **Tiling design**: Evenly partition the ultra-large global tensor data, allocate balanced independent data slices to each AI Core, precisely adapt to the SPMD multi-core parallel architecture, avoid single-core compute bottlenecks, and achieve global load balancing.

2. **Data copy-in**: Call the SIMD programming API to move the data to be computed from global memory in batches into high-speed on-chip local memory of the AI Core, such as L1 Buffer and UB.

3. **Data computation**: Schedule the Cube compute unit or Vector compute unit based on the operator type, and complete cube, vector, or mixed logic computation using on-chip cached data. For the same computation task, subsequent sections introduce multi-level differentiated programming APIs that can respectively address different development requirements such as development efficiency, ultimate performance, and C/C++ programming conventions.

4. **Data copy-out**: After computation is complete, write the result data in on-chip storage back to global memory to complete a single operator computation process.

> 📌 **Note**: The on-chip local storage of the AI Core is limited and cannot load an oversized tensor at once. In actual development, the strategy of "iterative block-based movement, batch computation, and result accumulation" is commonly adopted to complete global data computation, while pipelining is used to hide the data movement latency and improve overall computation efficiency.

### New-Architecture Dual-Mode Vector Computation Structure

Building on the traditional UB cache system, the new-generation Ascend 950PR/Ascend 950DT architecture opens up the programmability of registers, constructing a three-level memory hierarchy of "Global Memory → UB → Register". This gives rise to two vector computation modes tailored to different performance requirements, achieving full coverage of both general-purpose and ultimate-performance scenarios. The memory hierarchy is shown in the following figure:

**Figure 1**  SIMD Reg vector computation memory hierarchy  
![](../../../figures/simd_reg_vector_computation_memory_hierarchy_1.png "SIMD Reg vector computation memory hierarchy")

Based on the new three-level memory architecture, vector computation is divided into the general-purpose **memory vector computation** and the high-performance **Reg vector computation**:

- **Memory vector computation (traditional general-purpose mode)**: features full architecture compatibility, a simple and stable process, and strong generality, completing data caching and computation entirely on UB.

  - Data copy-in: Global Memory → UB

  - Vector computation: performs vector computation based on **UB**

  - Data copy-out: UB → Global Memory

- **Reg vector computation (high-performance mode)**: leverages the low-latency, high-bandwidth hardware advantages of registers and is designed specifically for ultimate performance optimization scenarios:

  - Data copy-in: global memory → UB → register (**Reg**)

  - Vector computation: perform vector computation based on **Reg**

  - Data copy-out: Register (**Reg**) → UB → global memory

Vector registers reside in the innermost layer of the Vector compute unit, offering the lowest memory access latency and the highest bandwidth. By exposing register-level programmability, they help developers fully unleash the peak compute performance of the hardware.

## Multi-level Programming API System: Layered Abstraction and Capability Differences

### Review of Multi-Level Operator Programming API Evolution

Early operator development centered on raw-pointer programming in C. Leveraging the low-level memory manipulation capability of C, developers could directly address and operate device memory through pointers, while native APIs that precisely map to low-level hardware instructions were provided to support hardware primitive calls such as vector computation, cube multiplication, and data movement. This model enables fine-grained control over hardware resources such as caches and registers, serving as the core solution for low-level ultimate performance optimization. It is widely used in the low-level implementation of core operators such as discrete, reduction, and cube operators.

As deep learning continues to evolve iteratively, the core carrier of AI computation has been upgraded to 4D/5D high-dimensional tensors (such as attention tensors in NLP models and feature map tensors in CV models). Traditional raw-pointer programming requires manually performing repetitive tasks such as memory offset computation, dimension splitting, and boundary checking, which not only leads to low development efficiency but also easily introduces stability issues such as out-of-bounds memory access and index errors.

Against this backdrop, the tensor abstraction based on C++ object-oriented features emerged. A tensor embeds tensor metadata such as memory layout and data type, where the layout includes dimensions (Shape) and strides, simplifying memory layout management and reducing the development cost of high-dimensional tensors. The industry has thus gradually formed the mainstream Tensor/Tile programming model: the core idea is to split a global tensor into regular, fixed-size data blocks (tiles), with the tile serving as the smallest unit of computation, storage, and hardware scheduling. Among them, tensor is a standardized mathematical and computational abstraction of multidimensional arrays, serving as the unified representation of various data types such as vectors, cubes, and high-dimensional feature maps in AI scenarios. It only describes the global logical structure and is not bound to hardware execution details. Tile, on the other hand, is the smallest scheduling, computation, and data movement granularity of tensor cores and vector cores on AI acceleration chips. As the physical-layer carrier of the programming model, it bridges global data and underlying hardware. By splitting a global tensor into tiles that match the hardware storage and compute specifications, and combining optimization techniques such as data movement, pipeline scheduling, and on-chip memory reuse, the ultimate hardware compute capability can be fully exploited and unleashed.

The current Tensor/Tile programming paradigm mainly includes two implementation paths: manual Tensor/Tile programming is generally implemented based on C/C++, where developers fully control the entire process of memory layout, tiling strategy, data movement, and pipeline scheduling; automated Tensor/Tile programming is generally implemented based on Python domain-specific languages (DSLs), such as [PyPTO](https://gitcode.com/cann/pypto), [TileLang](https://github.com/tile-ai/tilelang-ascend), and [Triton](https://github.com/openai/triton), where the AI compiler automatically completes memory allocation, resource reuse, task scheduling, and dependency synchronization, lowering the development barrier.

### Ascend C Multi-Level Programming APIs

Ascend C adheres to the core design principle of "standard C/C\+\+ syntax with minimal extensions", building a lightweight, high-performance low-level programming foundation that provides ultimate performance-oriented operator development capabilities for Ascend series chips. It also precisely adapts to the differentiated programming conventions of C and C\+\+ developers, achieving an optimal balance between fine-grained low-level hardware control and efficient high-level engineering development.

Based on the above design philosophy and common industry development practices, Ascend C natively provides two core programming systems: C APIs for pointer-based programming, and basic C++ APIs for tensor-based programming. In addition, drawing on the classic C/C++ queue pipeline design concept, Ascend C innovatively introduces the TPipe/TQue programming framework, which simplifies the memory scheduling, data synchronization, and pipeline management in heterogeneous computing scenarios, effectively lowering the barrier to developing high-performance operators.

Ascend C provides three tiers of graduated programmable APIs. All tiers support complete AI Core compute scheduling, with progressive capabilities and clear scenario adaptation across tiers. The specific differences are as follows:

| API Level | Language | Feature | Primary Use |
|----------|------|------|----------|
| **TPipe/TQue framework programming API** | **C++** | Based on **tensor** programming, automatically manages memory movement and synchronization through **TPipe/TQue**, shielding underlying details. | Uses the framework to automatically orchestrate data movement and computation, improving programming ease of use and development efficiency. |
| **Basic API** | **C++** | Based on **tensor** programming, provides **complete C++ programming capabilities**, allocates tensors through `LocalMemoryAllocator` and the like, with synchronization autonomously managed by the developer. | Autonomously manages synchronization and memory layout, adapts to C\+\+ tensor development conventions, exposes all underlying hardware capabilities, and supports fine-grained tuning and ultimate performance. |
| **Language extension layer SIMD API** | **C** | Based on **pointer** programming, provides **complete C programming capabilities**, manages local memory by declaring static arrays, with synchronization autonomously managed by the developer. | Autonomously manages synchronization and memory, adapts to C language development conventions, exposes all underlying hardware capabilities, and supports fine-grained tuning and ultimate performance. |

Developers can flexibly select the appropriate API level to complete operator development based on their own programming conventions, project development efficiency requirements, and performance optimization scenarios.

> 📌 **Tip** Ascend C uses GlobalTensor and LocalTensor to abstract the tensor/tile corresponding to global memory and on-chip local memory, respectively. In the current version, LocalTensor does not carry memory layout information. In later versions, LocalTensor in the tensor API will natively support layout management. For the specific release schedule, see [Ascend C DevKit Repository](https://gitcode.com/cann/asc-devkit).

## Summary

This chapter systematically describes the end-to-end technical system of AI Core operator development, from task distribution and hardware scheduling to code implementation. Computation tasks are distributed to multiple AI Cores for parallel execution based on the SPMD\+SIMD heterogeneous parallelism paradigm, and are then precisely scheduled to Cube units or Vector units for computation according to the operator's computation characteristics. Developers can flexibly select multi-level C/C++ programming APIs based on their own programming conventions and business scenarios to complete the full logic development of the operator kernel.

The subsequent chapters will progressively explain, layer by layer, the hardware architecture principles, kernel function definition, programming API specifications at each level, memory management mechanisms, data synchronization logic, hands-on development process, and performance tuning techniques. This helps developers quickly understand the core technologies and master the capability of developing high-performance, highly available AI Core operators.
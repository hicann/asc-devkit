# AI Core SIMT Programming Model Overview

<!-- md-trans-meta sourceCommit=22b76e1a8a68229c866dd2009081b9642a4559bf translatedAt=2026-10-28T09:22:36.410Z pushedAt=2026-11-03T07:08:03.569Z nl=2 -->

## Introduction

<!-- @ref: asc-devkit/res/docs/en/guide/programming_guide/programming_model/ai_core_simt_programming/overview_res.md#id1 -->

SIMT (Single Instruction Multiple Thread) programming is an important supplement to the AI Core programming approach, and is particularly suitable for scenarios such as discrete data access, complex control logic, and branch divergence. Ascend C supports an SIMT programming model consistent with industry practice, implementing data-parallel computation through thread-level parallelism. Each thread allows instructions to independently address and compute on data, thereby achieving higher programming flexibility. This chapter focuses on the **SIMT programming model** and follows the progressive logic of "macro architecture - thread scheduling - code implementation" to fully cover the entire SIMT operator development chain: it explains the execution mechanism of the multi-threaded parallel architecture, breaks down the underlying logic of Warp scheduling within a thread block, and completes the development of the operator kernel function (Kernel) computation logic.

## Heterogeneous Parallel Computing Core Model: SIMT Programming Paradigm

SIMT programming supports the industry-standard thread architecture: **Grid-Block-Thread**, with the thread as the smallest execution unit, achieving data-parallel computation through large-scale thread concurrency. The parallel logic, data partitioning strategy, and thread scheduling mechanism of all SIMT operators are implemented based on this programming paradigm.

**Three-level thread hierarchy**:
- **Grid**: Composed of multiple thread blocks, the built-in variable [gridDim](../../language_extension/simt_builtin_keywords.md#li20760123812911) indicates the number of enabled thread blocks. The Grid dimension is represented by the three-dimensional dim3 structure.
- **Thread Block**: Composed of several threads, the built-in variable [blockDim](../../language_extension/simt_builtin_keywords.md#li076017381191) indicates the number of threads enabled in a thread block (a thread block can enable up to 2048 threads). Each thread block is identified by the thread block index [blockIdx](../../language_extension/simt_builtin_keywords.md#li1676053814914).
- **Thread**: The smallest execution unit. Each thread has its own independent registers and can be identified by the thread index [threadIdx](../../language_extension/simt_builtin_keywords.md#li7760123814919), responsible for completing specific data computation tasks.

**Warp scheduling mechanism**:

Each thread block is divided into multiple Warps that are scheduled and executed in sequence. A Warp is a collection of threads that execute the same instruction, and each Warp contains 32 threads. Each AIV core contains multiple Warp Schedulers. When a Warp is blocked due to global memory access, the Warp Scheduler immediately switches to a ready Warp, ensuring that the compute units remain busy, thereby hiding memory access latency and maximizing hardware utilization.

> 📌 **Tip**: In a program based on the SIMT programming model, multiple thread blocks with the same structure are executed on the AIV core, and the total number of threads executed equals gridDim × blockDim. Each thread points to the same code logic but processes different data segments, achieving data parallelism.

## Hardware Foundation: SIMT Unit Architecture and Memory Hierarchy

The vector processing unit [AIV core](../../advanced_programming/hardware_implementation/basic_architecture.md) is the core hardware carrier of SIMT programming. It adopts an architecture of "Warp scheduler + compute unit + hierarchical storage system". This section describes the core SIMT hardware components and the memory hierarchy of SIMT programming in detail.

### Core Hardware Components

- **Warp scheduler**: Each AIV core contains multiple Warp schedulers, which are responsible for splitting and scheduling thread blocks. The Warp scheduler divides a thread block into multiple Warps and dispatches them in sequence, implementing efficient thread-level parallel scheduling.
- **Compute unit**: The vector compute unit in the AIV core is responsible for executing various vector operation instructions. The 32 threads in the same Warp execute the same instruction but operate on different data addresses, implementing single-instruction multiple-thread parallel computing.
- **Register file**: Each thread has an independent register space for storing local variables and intermediate computation results. The number of registers is affected by the number of threads in a thread block. The more threads there are, the fewer registers are available to each thread.
- **Shared memory**: The memory space shared by all threads in a thread block is located inside the AIV core. It features high bandwidth and low latency, and can be used as a user-managed cache to support data exchange between threads.


### SIMT Programming Memory Hierarchy

SIMT threads can access multiple memory spaces, forming a three-level memory hierarchy of "global memory - shared memory - register". The scope, lifecycle, and physical location of each level differ as follows:

| Memory Type | Thread Scope | Memory Modifier | Lifecycle | Physical Location | Feature |
|----------|-----------|---------|----------|---------|------|
| **Global Memory** | Grid | \_\_gm__ | Application | Device | Directly accessible by all threads, persistent storage |
| **Shared Memory (UB)** | Block | \_\_ubuf__ | Kernel Function | AIV Core | Shared within a thread block, high bandwidth and low latency |
| **Register** | Thread | NA | Kernel Function | AIV Core | Thread-private, compiler-managed, quantity affected by blockDim |


## SIMT Programming Interface Architecture

### SIMT Programming Interface Features

The SIMT programming interface follows the industry unified manner. Developers operate global memory and shared memory through the operators and interfaces provided by Ascend C to achieve full control over hardware capabilities. Meanwhile, SIMT programming also provides some advanced features for specific application scenarios. For details about the interfaces, see [SIMT-API](../../../../api/SIMT-API/SIMT-API.md).

Core features of the SIMT programming interface:
- **Built-in keyword extension**: Following the industry unified manner, it provides the `__global__` modifier and thread index variables such as `blockIdx`, `threadIdx`, `blockDim`, and `gridDim`.
- **Shared memory management**: It supports usage consistent with the industry. Static shared memory is allocated through the `__ubuf__` modifier, and dynamic memory application is also supported.
- **Synchronization interface support**: It supports synchronization methods consistent with the industry, providing synchronization interfaces such as `asc_syncthreads` and `asc_threadfence` to ensure the correctness of data access between threads.
- **Warp interface support**: It supports Warp interfaces consistent with the industry, providing Warp operation interfaces such as `asc_shfl`, `asc_reduce_max`, and `asc_all` for directly operating on warps.
- **Atomic operation support**: It supports usage consistent with the industry, providing atomic operation interfaces such as `asc_atomic_add`, `asc_atomic_sub`, and `asc_atomic_exch`.
- **Mathematical function support**: Following the industry unified manner, it provides computation interfaces such as `sin`, `cos`, `exp`, `__brev`, and `__clz`, and supports multiple data types such as `half`, `half2`, `bfloat16_t`, `bfloat16x2_t`, `float`, and `float2`.
- **Advanced feature support**: It currently provides the cooperative groups feature for business scenarios that require flexible control over grouping.

## Summary

This chapter systematically describes the full technical system of SIMT operators, from thread architecture and hardware scheduling to code implementation: computing tasks are distributed to AIV cores for parallel execution based on the Grid-Block-Thread three-level thread hierarchy, and data ranges are automatically mapped through thread indices to achieve large-scale concurrent thread computation. Based on the SIMT language programming interface, developers operate global memory and shared memory through pointers, and use synchronization mechanisms to ensure the correctness of data access between threads.

The following chapters provide in-depth details on the [Abstract Hardware Architecture](./abstract_hardware_architecture.md), [Thread Architecture](./thread_architecture.md), definition and invocation of the [Kernel Function](./kernel_function.md), management mechanisms of the [Memory Hierarchy](./memory_hierarchy.md), principles of the [Synchronization Mechanism](./synchronization.md), usage of [Atomic Operations](./atomic_operations.md), and the hands-on workflow of [Programming Examples](./programming_examples.md), helping developers quickly understand the core technologies and master the development of high-performance, highly available SIMT operators.

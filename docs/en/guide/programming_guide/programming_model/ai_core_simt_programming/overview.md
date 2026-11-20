# AI Core SIMT Programming Model Overview

<!-- md-trans-meta sourceCommit=22b76e1a8a68229c866dd2009081b9642a4559bf translatedAt=2026-11-09T11:09:22.701Z pushedAt=2026-11-19T06:32:19.982Z nl=2 -->

## Introduction

<!-- @ref: asc-devkit/res/docs/zh/guide/programming_guide/programming_model/ai_core_simt_programming/overview_res.md#id1 -->

SIMT (Single Instruction Multiple Thread) programming is an important complement to AI Core programming approaches, and is particularly suitable for scenarios such as discrete data access, complex control logic, and branch divergence. Ascend C supports an SIMT programming model consistent with industry standards, implementing data-parallel computation through thread-level parallelism. Each thread allows instructions to independently address and compute data, thereby achieving greater programming flexibility. This chapter focuses on the **SIMT programming model** and follows the progressive logic of "macro architecture - thread scheduling - code implementation" to fully cover the entire SIMT operator development chain: it explains the execution mechanism of the multi-threaded parallel architecture, breaks down the underlying logic of Warp scheduling within thread blocks, and completes the development of the computation logic of the operator kernel function.

## Core Model of Heterogeneous Parallel Computing: SIMT Programming Paradigm

SIMT programming supports the industry-standard thread architecture **Grid-Block-Thread**, which uses threads as the minimum execution unit and implements data-parallel computing through large-scale thread concurrency. The parallel logic, data partitioning strategy, and thread scheduling mechanism of all SIMT operators are implemented based on this programming paradigm.

**Three-level thread hierarchy**:
- **Grid**: Consists of multiple thread blocks. The built-in variable [gridDim](../../language_extension/simt_builtin_keywords.md#li20760123812911) indicates the number of enabled thread blocks. The Grid dimension is represented by the three-dimensional dim3 structure.
- **Thread Block**: Consists of a number of threads. The built-in variable [blockDim](../../language_extension/simt_builtin_keywords.md#li076017381191) indicates the number of threads enabled in a thread block (a thread block can enable up to 2048 threads). Each thread block is identified by the thread block index [blockIdx](../../language_extension/simt_builtin_keywords.md#li1676053814914).
- **Thread**: The minimum execution unit. Each thread has its own independent registers and can be identified by the thread index [threadIdx](../../language_extension/simt_builtin_keywords.md#li7760123814919). It is responsible for completing specific data computation tasks.

**Warp scheduling mechanism**:

Each thread block is divided into multiple warps that are scheduled and executed in sequence. A warp is a set of threads that execute the same instruction, and each warp contains 32 threads. Each AIV core contains multiple warp schedulers. When a warp is blocked due to global memory access, the warp scheduler immediately switches to a ready warp, keeping the compute units busy, thereby hiding memory access latency and maximizing hardware utilization.

> 📌 **Tip**: A program based on the SIMT programming model executes multiple thread blocks with the same structure on the AIV core, and the total number of threads executed equals gridDim × blockDim. Each thread points to the same code logic but processes different data fragments, implementing data parallelism.

## Hardware Foundation: SIMT Unit Architecture and Memory Hierarchy

The vector processing unit ([AIV core](../../advanced_programming/hardware_implementation/basic_architecture.md)) is the core hardware carrier of SIMT programming, adopting an architecture of "Warp scheduler + compute unit + hierarchical storage system". The following describes the core SIMT hardware components and the memory hierarchy of SIMT programming in detail.

### Core Hardware Components

- **Warp scheduler**: Each AIV core contains multiple warp schedulers, which are responsible for splitting and scheduling thread blocks. A warp scheduler divides a thread block into multiple warps and issues them for execution in sequence, implementing efficient thread-level parallel scheduling.
- **Compute unit**: The vector compute unit in an AIV core is responsible for executing various vector operation instructions. The 32 threads in the same warp execute the same instruction but operate on different data addresses, implementing single-instruction multiple-thread parallel computing.
- **Register file**: Each thread has an independent register space for storing local variables and intermediate computation results. The number of registers is affected by the number of threads in a thread block. The more threads there are, the fewer registers are available to each thread.
- **Shared memory**: The memory space shared by all threads in a thread block is located inside the AIV core. It features high bandwidth and low latency, and can be used as a user-managed cache to support data exchange between threads.


### SIMT Programming Memory Hierarchy

SIMT threads can access multiple memory spaces, forming a three-level memory hierarchy of "global memory - shared memory - registers". The scope, lifetime, and physical location of each level differ as follows:

| Memory Type | Thread Scope | Memory Modifier | Lifetime | Physical Location | Characteristics |
|----------|-----------|---------|----------|---------|------|
| **Global memory** | Grid | \_\_gm__ | Application | Device | Directly accessible by all threads, persistent storage |
| **Shared memory (UB)** | Block | \_\_ubuf__ | Kernel function | AIV core | Shared within a thread block, high bandwidth and low latency |
| **Registers** | Thread | NA | Kernel function | AIV core | Thread-private, managed by the compiler, quantity affected by blockDim |


## SIMT Programming API System

### SIMT Programming API Features

The SIMT programming API follows the industry-standard approach. Developers use the operators and APIs provided by Ascend C to operate global memory and shared memory, achieving full control over hardware capabilities. SIMT programming also provides some advanced features for specific application scenarios. For details about specific APIs, see [SIMT-API](../../../../api/SIMT-API/SIMT-API.md).

Key features of the SIMT programming API:
- **Built-in keyword extensions**: Following the industry-standard approach, it provides the `__global__` modifier and thread index variables such as `blockIdx`, `threadIdx`, `blockDim`, and `gridDim`.
- **Shared memory management**: It supports the industry-standard usage. Static shared memory is allocated through the `__ubuf__` modifier, and dynamic memory allocation is also supported.
- **Synchronization API support**: It supports the industry-standard synchronization approach and provides synchronization APIs such as `asc_syncthreads` and `asc_threadfence` to ensure the correctness of data access between threads.
- **Warp API support**: It supports the industry-standard Warp APIs and provides Warp operation APIs such as `asc_shfl`, `asc_reduce_max`, and `asc_all` for directly operating on warps.
- **Atomic operation support**: It supports the industry-standard usage and provides atomic operation APIs such as `asc_atomic_add`, `asc_atomic_sub`, and `asc_atomic_exch`.
- **Math function support**: Following the industry-standard approach, it provides computation APIs such as `sin`, `cos`, `exp`, `__brev`, and `__clz`, and supports multiple data types including `half`, `half2`, `bfloat16_t`, `bfloat16x2_t`, `float`, and `float2`.
- **Advanced feature support**: It currently provides the cooperative groups feature for scenarios that require flexible control over grouping.

## Summary

This chapter systematically describes the full technical system of SIMT operators, from thread architecture and hardware scheduling to code implementation. Computing tasks are distributed to AIV cores for parallel execution based on the three-level Grid-Block-Thread hierarchy, and data ranges are automatically mapped through thread indexes to achieve large-scale concurrent thread computing. Based on the SIMT language programming API, developers operate global memory and shared memory through pointers, and use synchronization mechanisms to ensure the correctness of data access between threads.

The following chapters provide in-depth details on the [Abstract Hardware Architecture](./abstract_hardware_architecture.md), [Thread Architecture](./thread_architecture.md), definition and invocation of the [Kernel Function](./kernel_function.md), [Memory Hierarchy](./memory_hierarchy.md) management mechanism, [Synchronization](./synchronization.md) principles, [Atomic Operations](./atomic_operations.md) usage, and [Programming Examples](./programming_examples.md) practical process, helping developers quickly understand the core technologies and master high-performance, highly available SIMT operator development techniques.

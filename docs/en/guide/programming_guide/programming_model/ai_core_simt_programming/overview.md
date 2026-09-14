# AI Core SIMT Programming Model Overview

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-20T11:33:38.487Z pushedAt=2026-08-26T03:07:03.155Z -->

## Introduction

Single Instruction Multiple Thread (SIMT) programming is an important supplement to AI Core programming, and is particularly suitable for scenarios such as discrete data access, complex control logic, and branch divergence. Ascend C supports an industry-consistent SIMT programming model that implements data-parallel computation through thread-level parallelism. Each thread allows instructions to independently address and compute data, thereby achieving higher programming flexibility. This chapter focuses on the **SIMT programming model** and follows the progressive logic of "macro architecture - thread scheduling - code implementation" to fully cover the SIMT operator development pipeline: it explains the execution mechanism of the multi-threaded parallel architecture, breaks down the underlying logic of warp scheduling within a thread block, and completes the development of the operator kernel computation logic.

## Heterogeneous Parallel Compute Core Model: SIMT Programming Paradigm

SIMT programming supports the industry-standard thread architecture: **Grid-Block-Thread**, which uses threads as the minimum execution unit and implements data-parallel computation through large-scale thread concurrency. The parallel logic, data partitioning strategy, and thread scheduling mechanism of all SIMT operators are implemented based on this programming paradigm.

**Three-level thread hierarchy**:

- **Grid**: Consists of multiple thread blocks. The built-in variable [gridDim](../../language_extension/SIMT-BuiltIn_keyword.md#griddim) indicates the number of enabled thread blocks. The Grid dimension is represented by the three-dimensional dim3 structure.

- **Thread Block**: Consists of several threads. The built-in variable [blockDim](../../language_extension/SIMT-BuiltIn_keyword.md#blockdim) indicates the number of threads enabled in a thread block (a thread block can enable up to 2048 threads). Each thread block is identified by the thread block index [blockIdx](../../language_extension/SIMT-BuiltIn_keyword.md#li1676053814914).

- **Thread**: Smallest execution unit. Each thread has its own independent registers and can be identified by the thread index [threadIdx](../../language_extension/SIMT-BuiltIn_keyword.md#li7760123814919). It is responsible for completing specific data computation tasks.

**Warp scheduling mechanism**:

Each thread block is divided into multiple warps that are scheduled and executed in sequence. A warp is a set of threads that execute the same instruction, and each warp contains 32 threads. Each AIV core contains multiple warp schedulers. When a warp is blocked due to global memory access, the warp scheduler immediately switches to a ready warp, keeping the compute units busy, thereby hiding memory access latency and maximizing hardware utilization.

> 📌 **Tip**: In a program based on the SIMT programming model, multiple thread blocks with the same structure are executed on the AIV core, and the total number of threads executed equals gridDim × blockDim. Each thread points to the same code logic but processes different data segments, achieving data parallelism.

## Hardware Foundation: SIMT Unit Architecture and Memory Hierarchy

The vector processing unit ([AIV core](../../advanced_programming/hardware_implementation/basic_architecture.md)) is the core hardware carrier of SIMT programming. It adopts an architecture of "warp scheduler + compute units + hierarchical storage system". The following describes the core hardware components of SIMT and the memory hierarchy of SIMT programming in detail.

### Core Hardware Components

- **Warp scheduler**: Each AIV core contains multiple warp schedulers, which are responsible for splitting and scheduling thread blocks. The warp scheduler divides a thread block into multiple warps and issues them for execution in sequence, implementing efficient thread-level parallel scheduling.

- **Compute unit**: The vector compute unit inside the AIV core is responsible for executing various vector operation instructions. The 32 threads within the same warp execute the same instruction but operate on different data addresses, implementing single-instruction multiple-thread parallel computation.

- **Register file**: Each thread has an independent register space for storing local variables and intermediate computation results. The number of registers is affected by the number of threads in a thread block; the more threads there are, the fewer registers are available to each thread.

- **Shared memory**: The memory space shared by all threads in a thread block is located inside the AIV core. It features high bandwidth and low latency, and can be used as a user-managed high-speed cache to support data exchange between threads.

### SIMT Programming Memory Hierarchy

SIMT threads can access multiple memory spaces, forming a three-level memory hierarchy of "global memory - shared memory - registers". The scope, lifecycle, and physical location of each level differ as follows:

| Memory Type | Thread Scope | Memory Modifier | Lifecycle | Physical Location | Characteristics |
|----------|-----------|---------|----------|---------|------|
| **Global memory** | Grid | \_\_gm__ | Application | Device | Directly accessible by all threads, persistent storage |
| **Shared memory (Unified Buffer)** | Block | \_\_ubuf__ | Kernel function | AIV core | Shared within a thread block, high bandwidth and low latency |
| **Registers** | Thread | N/A | Kernel function | AIV core | Thread-private, compiler-managed, count affected by blockDim |

## SIMT Programming API Framework

### SIMT Programming API Features

The SIMT programming API follows the industry-standard approach. Developers use the operators and APIs provided by Ascend C to operate global memory and shared memory, achieving full control over hardware capabilities. SIMT programming also provides some advanced features for specific application scenarios. For details about specific APIs, see [SIMT-API](../../../../api/SIMT-API/SIMT-API.md).

Key features of the SIMT programming API:

- **Built-in keyword extension**: Following the industry-standard approach, provides the `__global__` modifier and thread index variables such as `blockIdx`, `threadIdx`, `blockDim`, and `gridDim`.

- **Shared memory management**: Supports the same usage as the industry standard, allowing static shared memory to be allocated through the `__ubuf__` modifier, and also supports dynamic memory allocation.

- **Synchronization API support**: Supports synchronization methods consistent with industry practices, providing synchronization APIs such as `asc_syncthreads` and `asc_threadfence` to ensure correct data access between threads.

- **Warp API support**: Supports warp APIs consistent with industry practices, providing warp operation APIs such as `asc_shfl`, `asc_reduce_max`, and `asc_all` for directly operating on warps.

- **Atomic operation support**: Supports usage consistent with industry practices, providing atomic operation APIs such as `asc_atomic_add`, `asc_atomic_sub`, and `asc_atomic_exch`.

- **Math function support**: Following the industry-standard approach, provides computation APIs such as `sin`, `cos`, `exp`, `__brev`, and `__clz`, and supports multiple data types such as `half`, `half2`, `bfloat16_t`, `bfloat16x2_t`, `float`, and `float2`.

- **Advanced feature support**: Currently provides the cooperative group feature for scenarios that require flexible control over grouping.

## Summary

This chapter systematically describes the full technical chain of SIMT operators, from thread architecture and hardware scheduling to code implementation: computational tasks are distributed to AIV cores for parallel execution based on the three-level Grid-Block-Thread hierarchy, and data ranges are automatically mapped through thread indices to enable large-scale concurrent thread computation. Based on the SIMT language programming API, developers manipulate global memory and shared memory through pointers, and use synchronization mechanisms to ensure the correctness of data access between threads.

The following chapters provide in-depth details on the [abstract hardware architecture](./abstract_hardware_architecture.md), [thread architecture](./thread_architecture.md), [kernel function](./kernel_function.md) definition and call, [memory hierarchy](./memory_hierarchy.md) management mechanisms, [synchronization mechanism](./synchronization_mechanism.md) principles, [atomic operation](./atomic_operation.md) usage, and [programming example](./programming_example.md) hands-on process, helping developers quickly understand the core technologies and master high-performance, highly available SIMT operator development techniques.
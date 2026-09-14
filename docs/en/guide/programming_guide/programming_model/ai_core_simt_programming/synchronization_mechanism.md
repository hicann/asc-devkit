# Synchronization Mechanism<a name="ZH-CN_TOPIC_0000002554691683"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-20T11:40:21.770Z pushedAt=2026-08-26T03:07:09.059Z -->

SIMT is a single-instruction, multiple-thread programming model designed to hide memory access latency and improve compute throughput through concurrent execution of multiple threads. In the SIMT programming model, multiple threads within a thread block execute in parallel, and different threads may have different execution progress and memory access arrival order. When data dependencies exist between threads, synchronization APIs or memory fence APIs are required to constrain the execution order and memory visibility, preventing the reading of data whose writes have not completed. Synchronization APIs are mainly classified into two types:

- **Synchronization barrier**: requires all threads within the same scope to reach a specified position before the program can continue execution.

- **Memory fence**: constrains the memory access order of the calling thread before and after the fence, making the memory operations before the fence visible to other threads within the specified scope. A memory fence does not wait for other threads to reach the same position.

## API Overview

| Type | Representative API | Typical Use |
| --- | --- | --- |
| Synchronization barrier | `asc_syncthreads` | Phase synchronization within a thread block, for example, write to the Unified Buffer first and then read it collectively. |
| Memory fence | `asc_threadfence_block`, `asc_threadfence` | Constrains the memory access order and visibility of the current thread before publishing data or flags. |

For the detailed semantics, scope, usage examples, and precautions of synchronization APIs and memory fence APIs, see [Introduction to Synchronization and Memory Fence](../../../../api/SIMT-API/sync_and_memory_fence/sync_and_memory_fence.md).

## Usage Suggestions

- Use a synchronization barrier when all threads in a thread block need to complete a certain phase before continuing execution.

- Use a memory fence when you only need to ensure the memory write order and visibility of the current thread.

- Do not use a memory fence as a synchronization barrier. A memory fence does not wait for other threads, nor does it guarantee that other threads have reached the same position.

- Do not use a synchronization barrier as a cross-thread-block synchronization API. To share state between different thread blocks, design a synchronization protocol that combines Global Memory, atomic operations, and memory fences.

- When multiple threads write to the same address concurrently, neither a synchronization barrier nor a memory fence can replace an atomic operation. To avoid write conflicts, use [atomic operations](atomic_operation.md).
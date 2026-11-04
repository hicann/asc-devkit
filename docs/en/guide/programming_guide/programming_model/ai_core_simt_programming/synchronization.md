# Synchronization Mechanism<a name="ZH-CN_TOPIC_0000002554691683"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-28T09:25:20.646Z pushedAt=2026-11-03T07:08:03.568Z nl=2 -->

SIMT is a single-instruction, multiple-thread programming model designed to hide memory access latency and improve compute throughput through concurrent execution of multiple threads. In the SIMT programming model, multiple threads within a thread block execute in parallel, and different threads may have different execution progress and memory access arrival order. When data dependencies exist between threads, synchronization APIs or memory fence APIs are required to constrain the execution order and memory visibility, preventing the reading of data whose writes have not completed. Synchronization APIs are mainly classified into two types:

- **Synchronization barrier**: requires all threads within the same scope to reach a specified position before the program can continue execution.
- **Memory fence**: constrains the memory access order of the calling thread before and after the fence, making the memory operations before the fence visible to other threads within the specified scope. A memory fence does not wait for other threads to reach the same position.

## API Overview

| Type | Representative API | Typical Use |
| --- | --- | --- |
| Synchronization barrier | `asc_syncthreads` | Phase synchronization within a Thread Block, for example, writing to UB first and then reading it uniformly. |
| Memory fence | `asc_threadfence_block`, `asc_threadfence` | Constrains the memory access order and visibility of the current Thread before publishing data or flags. |

For the detailed semantics, scope, usage examples, and precautions of synchronization interfaces and memory fence interfaces, see [Synchronization and Memory Fence Introduction](../../../../api/SIMT-API/sync_and_memory_fence/sync_and_memory_fence_intro.md).

## Usage Recommendations

- Use a synchronization barrier when all threads in a thread block need to complete a certain phase before continuing execution.
- Use a memory fence when you only need to ensure the memory write order and visibility of the current thread.
- Do not use a memory fence as a synchronization barrier. A memory fence does not wait for other threads, nor does it guarantee that other threads have reached the same position.
- Do not use a synchronization barrier as a cross-thread-block synchronization API. To share state between different thread blocks, design a synchronization protocol that combines Global Memory, atomic operations, and memory fences.
- When multiple Threads concurrently write to the same address, neither synchronization barriers nor memory fences can replace atomic operations. To avoid write conflicts, use [Atomic Operations](atomic_operations.md) to ensure the atomicity of read-modify-write operations.

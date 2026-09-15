# Introduction to Synchronization and Memory Fences

<!-- md-trans-meta sourceCommit=e1d69e702c9c347bfbdba7ca2ed786dab14f1ccf translatedAt=2026-09-04T00:31:05.974Z pushedAt=2026-09-07T06:34:23.533Z -->

> **Prerequisites**: This document involves SIMT thread organization concepts such as Grid, Thread Block, Thread, and Warp. It is recommended that you first read [Thread Architecture](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simt_programming/thread_architecture.md) in the programming guide to understand the related prerequisites.

In a SIMT program, different Threads may have different execution progress and memory access arrival order. When data dependencies exist between threads, you need to use a synchronization interface or a memory fence interface to constrain the execution order and memory visibility.

Synchronization mechanisms are mainly divided into two categories:

-   **Synchronization barrier**: Requires all threads within the same scope to reach a specified position before the program can continue execution.
-   **Memory fence**: Constrains the memory access order of the calling thread before and after the fence, making the memory operations before the fence visible to other threads within the specified scope. A memory fence does not wait for other threads to reach the same position.

## API List

| Type | Interface Name | Scope | Whether Blocks Threads | Description |
| --- | --- | --- | --- | --- |
| Synchronization barrier | [asc\_syncthreads](sync_interface/asc_syncthreads.md) | Current Thread Block | Yes | Waits for all threads in the current Thread Block to reach this synchronization point, and ensures that memory operations before the synchronization point are visible to threads within the block. |
| Memory fence | [asc\_threadfence\_block](memory_fence/asc_threadfence_block.md) | Current Thread Block | No | Ensures that the memory operations of the calling thread before the fence are visible, in order, to other threads in the current Thread Block. |
| Memory Fence | [asc\_threadfence](memory_fence/asc_threadfence.md) | Global Scope | No | Ensures that the global memory and shared memory write operations of the calling thread before the fence are visible, in order, to other threads. |

## Synchronization Barrier

`asc_syncthreads` is used for phase synchronization within a Thread Block. A typical scenario is that multiple Threads first write shared data to the Unified Buffer, and then uniformly enter the next phase to read the data.

```cpp
__global__ __launch_bounds__(256) void block_reduce(float *out, const float *in)
{
    __ubuf__ float buf[256];
    uint32_t tid = threadIdx.x;

    buf[tid] = in[blockIdx.x * blockDim.x + tid];

    // Wait for all Threads in the current Thread Block to finish writing to buf.
    asc_syncthreads();

    if (tid == 0) {
        float sum = 0.0f;
        for (uint32_t i = 0; i < blockDim.x; ++i) {
            sum += buf[i];
        }
        out[blockIdx.x] = sum;
    }
}
```

Note the following when using `asc_syncthreads`:

-   All Threads in the same Thread Block must reach this synchronization point; otherwise, the Threads that have already reached the synchronization point will keep waiting, causing the program to fail to continue execution.
-   Avoid calling `asc_syncthreads` in a branch unless you can guarantee that all Threads in the current Thread Block will enter that branch.
-   `asc_syncthreads` only synchronizes Threads within the current Thread Block and cannot be used for global synchronization across different Thread Blocks.

## Memory Fence

A memory fence constrains the memory access order of the calling thread. It addresses visibility issues such as "a thread writes data first and then publishes a status or flag", but it does not block other threads or make other threads wait automatically.

A typical producer-consumer scenario is as follows:

```cpp
data[idx] = value;

// Ensure that the data write is visible to other threads before the subsequent flag update.
asc_threadfence();

asc_atomic_exch(ready, 1U);
```

In the scenario above, `asc_threadfence` ensures that the data write performed by the calling thread before the fence becomes visible to other threads before the flag update. The consumer thread still needs to determine the `ready` status through polling, atomic operations, or other synchronization methods; the memory fence itself does not wait for the consumer thread.

The difference between `asc_threadfence_block` and `asc_threadfence` lies in their scope:

-   `asc_threadfence_block` constrains the data visibility order within a Thread Block, and is suitable for scenarios involving shared data within a block and Unified Buffer collaboration.
-   `asc_threadfence` constrains the data visibility order over a larger scope, and is suitable for scenarios where data or flags are published through Global Memory.

## Recommendations

-   Use `asc_syncthreads` when all Threads in a Thread Block need to complete a certain phase before execution continues.
-   Use `asc_threadfence_block` or `asc_threadfence` when only the memory write order and visibility of the current Thread need to be guaranteed.
-   Do not use memory fences as synchronization barriers. A memory fence does not wait for other threads, nor does it guarantee that other threads have reached the same point.
-   Do not use `asc_syncthreads` as a cross-thread-block synchronization interface. If different thread blocks need to share state, design a synchronization protocol that combines Global Memory, [Atomic Operation](../atomic_operations/atomic_operations_149.md), and memory fences.
-   When multiple threads concurrently write to the same address, neither `asc_syncthreads` nor memory fences can replace atomic operations. Avoid write conflicts, or use atomic operations to ensure the atomicity of read-modify-write processes.

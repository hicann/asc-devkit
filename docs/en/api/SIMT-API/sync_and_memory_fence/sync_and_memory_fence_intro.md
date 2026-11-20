# Introduction to Synchronization and Memory Fences

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-27T03:53:27.603Z pushedAt=2026-10-27T09:21:55.744Z nl=2 -->

In a SIMT program, different threads may progress at different rates and their memory accesses may arrive in different orders. When data dependencies exist between threads, synchronization APIs or memory fence APIs are required to constrain the execution order and memory visibility.

Synchronization mechanisms fall into two categories:

- **Synchronization barrier**: Requires all threads within the same scope to reach a specified point before the program can continue execution.
- **Memory fence**: Constrains the order of memory accesses before and after the fence for the calling thread, making the memory operations before the fence visible to other threads within the specified scope. A memory fence does not wait for other threads to reach the same point.

## API List

| Type | API | Scope | Blocks Threads | Description |
| --- | --- | --- | --- | --- |
| Synchronization barrier | [asc\_syncthreads](sync_interface/asc_syncthreads.md) | Current Thread Block | Yes | Waits for all threads within the current Thread Block to reach this synchronization point, and guarantees that the memory operations before the synchronization point are visible to the threads within the block. |
| Memory fence | [asc\_threadfence\_block](memory_fence/asc_threadfence_block.md) | Current Thread Block | No | Guarantees that the memory operations of the calling thread before the fence are visible, in order, to other threads within the current Thread Block. |
| Memory fence | [asc\_threadfence](memory_fence/asc_threadfence.md) | Global scope | No | Guarantees that the global memory and shared memory write operations of the calling thread before the fence are visible, in order, to other threads. |

## Synchronization barrier

`asc_syncthreads` is used for phase synchronization within a Thread Block. A typical scenario is that multiple threads first write shared data in the Unified Buffer (UB), and then uniformly enter the next phase to read the data.

```cpp
__global__ __launch_bounds__(256) void block_reduce(float *out, const float *in)
{
    __ubuf__ float buf[256];
    uint32_t tid = threadIdx.x;

    buf[tid] = in[blockIdx.x * blockDim.x + tid];

    // Wait until all threads in the current Thread Block finish writing to buf.
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

- All threads in the same Thread Block must reach the synchronization point. Otherwise, threads that have already reached the synchronization point keep waiting, causing the program to fail to continue execution.
- Avoid calling `asc_syncthreads` in a branch unless you can guarantee that all threads in the current Thread Block enter that branch.
- `asc_syncthreads` synchronizes only threads within the current Thread Block and cannot be used for global synchronization across different Thread Blocks.

## Memory fence

A memory fence constrains the memory access order of the calling thread. It addresses visibility issues such as "a thread writes data first and then publishes a status or flag," but it does not block other threads, nor does it make other threads wait automatically.

A typical producer-consumer scenario is as follows:

```cpp
data[idx] = value;

// Ensure that the data write becomes visible to other threads before the subsequent flag update.
asc_threadfence();

asc_atomic_exch(ready, 1U);
```

In the scenario above, `asc_threadfence` guarantees that the data write performed by the calling thread before the fence becomes visible to other threads before the flag update. The consumer thread still needs to determine the `ready` status through polling, atomic operations, or other synchronization methods; the memory fence itself does not wait for the consumer thread.

The difference between `asc_threadfence_block` and `asc_threadfence` lies in their scope:

- `asc_threadfence_block` constrains the data visibility order within a Thread Block, and is suitable for scenarios involving shared data within a block and UB collaboration.
- `asc_threadfence` constrains the data visibility order over a larger scope, and is suitable for scenarios where data or flags are published through Global Memory.

## Usage Recommendations

- Use `asc_syncthreads` when all threads within a Thread Block must complete a certain stage before execution continues.
- Use `asc_threadfence_block` or `asc_threadfence` when you only need to guarantee the memory write order and visibility of the current thread.
- Do not use memory fences as synchronization barriers. A memory fence does not wait for other threads, nor does it guarantee that other threads have reached the same point.
- Do not use `asc_syncthreads` as a cross-Thread Block synchronization API. If state needs to be shared between different Thread Blocks, design a synchronization protocol that combines Global Memory, [atomic operations](../atomic_operations/atomic_operations.md), and memory fences.
- When multiple threads write to the same address concurrently, neither `asc_syncthreads` nor memory fences can replace atomic operations. Avoid write conflicts, or use atomic operations to ensure the atomicity of read-modify-write operations.

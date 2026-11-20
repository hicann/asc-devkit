# Warp Functions

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-27T04:17:36.854Z pushedAt=2026-10-27T09:21:55.797Z nl=2 -->

A warp is the basic unit of SIMT thread execution and scheduling. Threads within a thread block are divided into multiple warps by linear thread index, and a warp currently contains 32 threads. Threads within the same warp execute the same instruction stream, but each thread has its own registers and lane ID, allowing it to process different data addresses and branch paths.

Warp functions are used for lightweight thread cooperation within the same warp. Common capabilities include conditional voting, register data exchange, intra-warp reduction, and lane information query. Compared with data exchange through the Unified Buffer (UB), warp functions typically perform register-level communication directly within the warp, making them suitable for small-scale, low-overhead thread cooperation scenarios.

## Basic Concepts

| Concept | Description |
| --- | --- |
| Warp | A group of threads in SIMT execution. A warp currently contains 32 threads. |
| Lane | The execution position corresponding to a single thread within a warp. |
| Lane ID | The index of a thread within the current warp, ranging from 0 to 31. |
| Active thread | A thread that actually participates in execution on the current instruction path. During branch divergence, threads that do not enter the current path do not participate in warp function computation on that path. |
| Branch divergence | Different threads within the same warp enter different branch paths. The hardware executes different paths in batches, and threads that are not executing the current path remain inactive. |

## Warp Function Classification

| Category |  Description |
| --- |  --- |
| Warp Vote functions | Aggregate the condition values of active threads in a warp to obtain all-true, any-true, a bitmap, or the active thread mask. |
| Warp Shfl functions | Exchange register data by Lane ID within a warp, which can be used for communication between neighboring threads, scanning, and reduction. |
| Warp Reduce functions | Perform sum, maximum, or minimum reduction on the input values of active threads in a warp. |
| Lane ID Functions | Obtain the Lane ID of the current thread or generate a Lane Mask based on the Lane ID. |

## Typical Scenarios

### Conditional Aggregation Within a Warp

Vote functions are suitable for determining whether any thread within the same Warp satisfies a condition, or for compressing the condition result of each Lane into a bit mask.

```cpp
uint32_t active_mask = asc_activemask();
uint32_t hit_mask = asc_ballot(value > threshold);
int32_t has_hit = asc_any(value > threshold);
```

### Register Exchange Within a Warp

Shfl functions can directly read the register values of other lanes in the same warp, reducing the overhead of transferring data through UB shared memory.

```cpp
int32_t lane = laneid();
int32_t next_value = asc_shfl_down(value, 1);
```

When using Shfl functions, ensure that the target lane is active. If the target lane is inactive, the read result may be an uninitialized value.

### Reduction Within a Warp

Reduce functions are used to quickly compute the sum, maximum, or minimum within the same Warp.

```cpp
float warp_sum = asc_reduce_add(value);
float warp_max = asc_reduce_max(value);
```

For floating-point reduction, the computation order of the reduction tree may differ from sequential accumulation, and the low-order rounding results may differ.

## Usage Recommendations

-   Warp functions take effect only within the current Warp and cannot be used for data synchronization across Warps or thread blocks.
-   Warp functions execute for active threads. If branch divergence exists in the code, verify that the set of Lanes participating in the computation is as expected.
-   When Shfl functions read register values from other Lanes, avoid reading data from inactive Lanes.
-   Reduce functions are suitable for small-scale reduction within a Warp; reduction across Warps or thread blocks typically requires combining UB, synchronization mechanisms, or atomic operations.
-   Warp functions are not equivalent to memory fences. To constrain memory visibility or stage synchronization within a thread block, use the APIs described in [Synchronization and Memory Fence](../sync_and_memory_fence/sync_and_memory_fence.md).

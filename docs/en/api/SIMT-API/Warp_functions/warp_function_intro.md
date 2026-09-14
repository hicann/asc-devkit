# Warp Functions

<!-- md-trans-meta sourceCommit=e1d69e702c9c347bfbdba7ca2ed786dab14f1ccf translatedAt=2026-09-08T01:17:42.247Z -->

> **Prerequisites**: This document involves SIMT thread organization concepts such as Grid, Thread Block, Thread, and Warp. It is recommended that you first read [Thread Architecture](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/programming_guide/programming_model/ai_core_simd_programming/thread_architecture.md) in the programming guide to understand the relevant prerequisites.

A warp is the basic unit of SIMT thread execution and scheduling. Threads within a thread block are divided into multiple warps according to their linear thread IDs, and a warp currently contains 32 threads. Threads within the same warp execute the same instruction stream, but each thread has its own registers and lane ID, allowing it to process different data addresses and branch paths.

Warp functions are used for lightweight thread cooperation within the same warp. Common capabilities include conditional voting, register data exchange, intra-warp reduction, and lane information query. Compared with data exchange through the Unified Buffer, warp functions typically perform register-level communication directly within the warp, making them suitable for small-scale, low-overhead thread cooperation scenarios.

## Basic Concepts

| Concept | Description |
| --- | --- |
| Warp | A thread group in SIMT execution. A warp currently contains 32 threads. |
| Lane | The execution position corresponding to a single thread within a warp. |
| Lane ID | The number of a thread within its current warp, ranging from [0, 31]. |
| Active thread | A thread that actually participates in execution on the current instruction path. During branch divergence, threads that do not enter the current path do not participate in warp function computation on that path. |
| Branch divergence | Different threads within the same warp enter different branch paths. The hardware executes different paths in batches, and threads that are not executing the current path remain inactive. |

## Warp Function Categories

| Category | Description |
| --- | --- |
| Warp Vote functions | Aggregates the condition values of active threads within a warp to obtain all-true, any-true, bitmap, or active thread mask. |
| Warp Shfl functions | Exchanges register data within a warp by lane ID, and can be used for neighboring thread communication, scanning, and reduction. |
| Warp Reduce functions | Performs sum, maximum, or minimum reduction on the input values of active threads within a warp. |
| Lane ID functions | Obtains the lane ID of the current thread or generates a lane mask based on the lane ID. |

## Typical Scenarios

### Conditional Aggregation Within a Warp

Vote functions are suitable for determining whether any thread in the same Warp satisfies a condition, or for compressing the condition result of each Lane into a bit mask.

```cpp
uint32_t active_mask = asc_activemask();
uint32_t hit_mask = asc_ballot(value > threshold);
int32_t has_hit = asc_any(value > threshold);
```

### Register Exchange Within a Warp

**Shfl** functions can directly read the register values of other lanes within the same warp, reducing the overhead of transferring data through UB shared memory.

```cpp
int32_t lane = laneid();
int32_t next_value = asc_shfl_down(value, 1);
```

When using **Shfl** functions, ensure that the target lane is active. If the target lane is inactive, the read result may be an uninitialized value.

### Reduction Within a Warp

The **Reduce** functions are used to quickly compute the sum, maximum, or minimum within the same **Warp**.

```cpp
float warp_sum = asc_reduce_add(value);
float warp_max = asc_reduce_max(value);
```

For floating-point reduction, the computation order of the reduction tree may differ from sequential accumulation, so the low-order rounding results may differ.

## Usage Recommendations

-   **Warp** functions take effect only within the current **Warp** and cannot be used for data synchronization across **Warps** or across thread blocks.
-   **Warp** functions execute for active threads. If the code has branch divergence, confirm that the set of **Lanes** participating in the computation is as expected.
-   When **Shfl** functions read register values from other **Lanes**, avoid reading data from inactive **Lanes**.
-   **Reduce** functions are suitable for small-scale reduction within a **Warp**; reduction across **Warps** or thread blocks usually requires combining **Unified Buffer**, synchronization mechanisms, or atomic operations.
-   **Warp** functions are not equivalent to memory barriers. When you need to constrain memory visibility or synchronize phases within a thread block, use the interfaces related to [synchronization and memory fence](../sync_and_memory_fence/sync_and_memory_fence.md).

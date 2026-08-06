# asc_atomic_add Interface Performance Comparison Sample

## Overview

Taking the [asc_atomic_add()](../../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md) atomic add as an example, this sample uses five groups of controlled experiments to show how the memory tier (Global Memory / Unified Buffer), the organization of the atomic operation (direct atomic add / intra-block accumulation followed by GM writeback), whether the return value is used, the contention intensity on the same address, and the data type (int32 / int64) affect the performance of atomic add, and provides corresponding optimization guidance.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Versions

- \>= CANN 9.1.0

## Directory Structure

```text
├── atomic_add_perf
│   ├── figures                // Image resources used in the README
│   ├── CMakeLists.txt         // Sample build script
│   ├── atomic_add_perf.asc    // Sample source code
│   ├── README.md
│   └── README_en.md
```

## Sample Description

This sample contains 20 scenarios in total, testing the performance of `asc_atomic_add()` under the following usage conditions:

- Different memory tiers (Global Memory, Unified Buffer).
- Different atomic-accumulation organizations (direct atomic add, intra-block UB accumulation followed by GM writeback).
- Whether the return value is used.
- Contention intensity on the same address.
- Data type (int32, int64).

The 20 scenarios are grouped into 5 controlled experiments (Case 1~5) for performance comparison, ultimately providing the performance characteristics of `asc_atomic_add()`. The sample selects one scenario to test via `SCENARIO_NUM`; each run launches only one target kernel.

**Five controlled groups**:

| Case | Dimension | Scenarios |
|:---:|---|---|
| Case 1 | Memory tier under a single thread block: Global Memory → Unified Buffer | Scenario 1 ↔ Scenario 2 |
| Case 2 | Memory tier under multiple thread blocks: Global Memory → Unified Buffer | Scenario 3 ↔ Scenario 4 |
| Case 3 | Return-value usage: with return value → without return value | Scenario 5 ↔ Scenario 6 (GM)<br>Scenario 7 ↔ Scenario 8 (UB) |
| Case 4 | Contention intensity on the same address: target_count 12288→1024→32→1 (weak to strong) | Scenario 9→10→11→12 (int32)<br>Scenario 13→14→15→16 (int64) |
| Case 5 | Data-type trade-off: int32_t → int64_t | Scenario 17 ↔ Scenario 18 (no return value)<br>Scenario 19 ↔ Scenario 20 (with return value) |

## Sample Implementation

This section analyzes the impact of memory tier, atomic organization, return value, same-address contention intensity, and data-type trade-off on atomic-add performance group by group (Case 1~5). Each group changes only one attribute while keeping the others identical, and provides the corresponding `msopprof` measured data and root-cause analysis.

### Performance Metric

| Metric | Description |
| --- | --- |
| Task Duration (μs) | Total task duration, including the time scheduled onto the accelerator, the execution time on the accelerator, and the response/completion time |


---

### Case 1: Memory-tier performance difference (single-block scenario)

**Goal**: Compare the performance of an atomic add accessing Global Memory versus Unified Buffer, when all threads contend for the same address.

**Scenario configuration**:

| Scenario | Memory | Data Type | Return Value | Scale (Block×Thread) | Atomic-add target address count |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 1 | Global Memory | int32_t | No | 1×1024 | 1 |
| 2 | Unified Buffer | int32_t | No | 1×1024 | 1 |

**Core implementation**: Launch 1 thread block with 1024 threads per block. In the GM scenario, the 1024 threads perform an atomic add on the same GM address; in the UB scenario, the 1024 threads perform an atomic add on the same UB address. Case 1 only compares the overhead of the interface accessing GM versus UB; the UB scenario does not perform correctness validation.

```cpp
// Scenario 1: GM single address, all threads contend for the same GM counter
__global__ void atomic_add_gm_single_no_return(int32_t* counter)
{
    asc_atomic_add(counter, 1);  // 1024 threads access the same GM address
}

// Scenario 2: UB single address, all threads call the atomic-add interface on the same UB address
__global__ void atomic_add_ub_local_no_return()
{
    __ubuf__ int32_t local[1];
    asc_atomic_add(&local[0], 1);  // 1024 threads access the same UB address
}
```

**Performance data**:

| Scenario | Memory | Task Duration (μs) |
|:---:|:---:|---:|
| 1 | Global Memory | 87.60 |
| 2 | Unified Buffer | 1.84 |

**Analysis**:

The performance data reveals one phenomenon:

**Atomic operations on UB are significantly faster than GM atomic operations.** With 1024 threads contending for the same address, the GM scenario takes about 87.60 μs and the UB scenario about 1.84 μs — the former is roughly **48×** of the latter.

The two scenarios have identical thread count, data type, return-value usage, and target address count; the only variable is the memory location of the atomic add. To explain this phenomenon, we must first understand the principle of atomic add and the storage locations of GM and UB.

**Principle 1: Atomic operations on the same address can only queue serially; they cannot execute in parallel.**

An atomic add must complete three steps: "read old value → compute new value → write back new value." If multiple threads update the same address simultaneously without protection, multiple threads may read the same old value, and later writebacks overwrite earlier ones, causing some additions to be lost. Atomic operations prevent such loss by guaranteeing that "read–compute–write" is completed as an indivisible whole. Therefore, multiple atomic operations on the same address cannot execute in parallel; they must execute serially.

**Principle 2: GM and UB have different atomic-access paths and processing overhead.**

GM is the global device memory located outside the AI Core, with a longer access path; UB is the shared memory located inside the AI Core, with a shorter access path and lower latency, so the per-operation processing overhead of an atomic operation on UB is lower than on GM.
In this same-address-contention scenario, the 1024 atomic operations on both GM and UB must execute serially, and the difference in processing overhead accumulates with each serial execution, ultimately making the total time on GM significantly higher than on UB.

**Conclusion**: Atomic accumulations that can be done on UB should avoid being done on GM.

---

### Case 2: Memory-tier performance difference (multi-block scenario)

**Goal**: When multiple thread blocks contend for the same GM address simultaneously, compare two organizations: "all threads directly atomic-add to the same GM address" versus "first accumulate within each block's own UB, then have each block write back to GM."

**Scenario configuration**:

| Scenario | Memory | Data Type | Return Value | Scale (Block×Thread) | Atomic-add target address count |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 3 | Global Memory | int32_t | No | 8×1024 | 1 |
| 4 | Unified Buffer | int32_t | No | 8×1024 | 1 |

**Core implementation**: Launch 8 thread blocks of 1024 threads each (8192 threads in total). In the GM scenario, the 8192 threads directly perform an atomic add on the same GM address; in the UB scenario, each block first accumulates on its own UB array `local[1]`, then each block performs one GM atomic-add writeback.

```cpp
// Scenario 3: GM single address, 8192 threads all contend for the same GM counter
__global__ void atomic_add_gm_single_no_return(int32_t* counter)
{
    asc_atomic_add(counter, 1);
}

// Scenario 4: Each block first accumulates in its own UB, then thread 0 writes back to GM
__global__ void atomic_add_ub_block_accum_no_return(int32_t* gm_result)
{
    __ubuf__ int32_t local[1];
    if (threadIdx.x == 0) {
        local[0] = 0;
    }
    asc_syncthreads();

    asc_atomic_add(&local[0], 1);  // 1024 threads access this block's UB address
    asc_syncthreads();

    if (threadIdx.x == 0) {
        asc_atomic_add(gm_result, local[0]);  // only 1 GM atomic add per block
    }
}
```

**Performance data**:

| Scenario | Organization | Task Duration (μs) |
|:---:|:---:|---:|
| 3 | GM direct atomic add | 351.47 |
| 4 | UB intra-block accumulation + GM writeback | 2.25 |

**Analysis**:

The two methods perform the same total number of atomic adds, yet their times differ by roughly **156×** (351.47 μs vs 2.25 μs). The gap comes from two compounding factors:

- **Parallel vs serial.** Atomic operations landing on the same address can only execute serially (see Case 1 for the principle). GM is accessible by threads of all thread blocks; in scenario 3, 8192 atomic adds concentrate on a single GM address and must execute serially. UB is accessible only by threads within the current thread block; different thread blocks use their own UB. In scenario 4, each block first accumulates on UB, so same-address atomic operations are limited to the 1024 threads within the same block, and the 8 blocks' UB atomic operations do not contend with each other and can truly execute in parallel; only the final 8 GM writebacks need serial execution.
- **UB atomic operations outperform GM.** The latency of atomic add on UB is far lower than on GM (see Case 1). Therefore, although scenario 4 adds UB initialization, `asc_syncthreads` synchronization, and GM writeback, the vast majority of atomic adds are done on UB, with only 8 landing on GM.

These two factors work together: the vast majority of atomic adds are not only moved to the faster UB but also split across thread blocks for parallel execution. The gains from both far exceed the overhead of UB initialization, synchronization, and GM writeback, so scenario 4 is still significantly faster than scenario 3 overall.

**Conclusion**: When multiple thread blocks need to perform same-address atomic accumulation, it is not recommended to have every thread directly perform an atomic add on GM — this forces all threads to queue serially on the same GM address. It is preferable to first complete intra-block accumulation in each block's UB in parallel, then have each block write back to GM once. This both moves most GM atomic operations to the better-performing UB and splits them into parallel tasks.

---

### Case 3: Return-value overhead

**Goal**: Compare the performance of `asc_atomic_add()` with and without using the return value, on both GM and UB.

**Scenario configuration**:

| Scenario | Memory | Data Type | Return Value | Scale (Block×Thread) | Atomic-add target address count |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 5 | Global Memory | int32_t | Yes | 1×2048 | 1 |
| 6 | Global Memory | int32_t | No | 1×2048 | 1 |
| 7 | Unified Buffer | int32_t | Yes | 1×2048 | 1 |
| 8 | Unified Buffer | int32_t | No | 1×2048 | 1 |

**Core implementation**: Launch 1 thread block with 2048 threads per block, testing with and without return value on both GM and UB. Except for whether the return value is used, all other conditions are kept identical. When the return value is used, the old value is written to a thread-local `volatile` variable, preventing the return-value path from being optimized away by the compiler.

```cpp
// Scenario 5: GM single address, with return value
__global__ void atomic_add_gm_single_return(int32_t* counter)
{
    volatile int32_t old_value_sink = asc_atomic_add(counter, 1);
    (void)old_value_sink;
}

// Scenario 6: GM single address, without return value
__global__ void atomic_add_gm_single_no_return(int32_t* counter)
{
    asc_atomic_add(counter, 1);
}

// Scenario 7: UB single address, with return value
__global__ void atomic_add_ub_local_return()
{
    __ubuf__ int32_t local[1];
    volatile int32_t old_value_sink = asc_atomic_add(&local[0], 1);
    (void)old_value_sink;
}

// Scenario 8: UB single address, without return value
__global__ void atomic_add_ub_local_no_return()
{
    __ubuf__ int32_t local[1];
    asc_atomic_add(&local[0], 1);
}
```

**Performance data**:

| Memory | With return value (μs) | Without return value (μs) | Ratio |
|:---:|---:|---:|:---:|
| GM int32 (Scenario 5, 6) | 421.59 | 159.88 | 2.64× |
| UB int32 (Scenario 7, 8) | 3.17 | 2.76 | 1.15× |

**Analysis**:

The impact of whether the return value is used differs significantly between GM and UB:

- **GM int32**: Without the return value, about 159.88 μs; with the return value, about 421.59 μs — a difference of roughly **2.6×**.
- **UB int32**: Without the return value, about 2.76 μs; with the return value, about 3.17 μs — only a difference of roughly **1.15×**.

The reason for this difference is: whether `asc_atomic_add()` uses the return value causes the compiler to generate **two different atomic-add instructions** — without the return value, the compiler selects the better-performing instruction; with the return value, the generated instruction has higher overhead. This instruction-level performance difference is significantly amplified on GM but small on UB, so the impact of return-value usage on time is far greater on GM than on UB.

**Conclusion**: It is recommended to prefer the without-return-value approach when the business logic allows it.

---

### Case 4: Contention intensity on the same address

**Goal**: Compare the impact on atomic-add performance as the contention intensity on the same GM address increases from weak to strong.

**Scenario configuration**:

| Scenario | Memory | Data Type | Return Value | Scale (Block×Thread) | Atomic-add target address count |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 9 | Global Memory | int32_t | Yes | 48×256 | 12288 |
| 10 | Global Memory | int32_t | Yes | 48×256 | 1024 |
| 11 | Global Memory | int32_t | Yes | 48×256 | 32 |
| 12 | Global Memory | int32_t | Yes | 48×256 | 1 |
| 13 | Global Memory | int64_t | Yes | 48×256 | 12288 |
| 14 | Global Memory | int64_t | Yes | 48×256 | 1024 |
| 15 | Global Memory | int64_t | Yes | 48×256 | 32 |
| 16 | Global Memory | int64_t | Yes | 48×256 | 1 |

**Core implementation**: Launch 48 thread blocks of 256 threads each (12288 threads in total). Each thread performs one atomic add of 1, writing the result to a region in GM consisting of several contiguous addresses. The `target_count` controls the number of target addresses in GM participating in the atomic add, to construct different levels of address contention. `target_count` is set to four levels: 12288, 1024, 32, and 1; the smaller the `target_count`, the more threads contend for the same address, and thus the stronger the contention. Both `int32_t` and `int64_t` cover all four levels.


> [!NOTE]
> This group of scenarios uniformly uses the return value: as shown by Case 3, when the return value is not used, int32 triggers an instruction optimization whose time is dominated by that optimization, masking the effects of contention intensity and target-address distribution within the 128B range; therefore, the int32 cases need to use the return-value version. For consistency, int64 also uses the return value. This makes the time difference between int32 and int64 reflect only the performance differences caused by contention intensity and target-address distribution.

```cpp
// Fixed thread scale, only target_count changes; the smaller target_count, the more threads land on the same address, the stronger the contention
__global__ void atomic_add_gm_dense_i32_return(int32_t* counters, uint64_t target_count)
{
    volatile int32_t old_value_sink =
        asc_atomic_add(counters + ((blockIdx.x * blockDim.x + threadIdx.x) % target_count), 1);
    (void)old_value_sink;
}
```

**Performance data**:

| Data type | target_count | Threads contending per address | Task Duration (μs) |
|:---:|:---:|:---:|---:|
| int32 (Scenario 9) | 12288 | 1 (no same-address contention) | 15.54 |
| int32 (Scenario 10) | 1024 | 12 | 111.30 |
| int32 (Scenario 11) | 32 | 384 | 2233.62 |
| int32 (Scenario 12) | 1 | 12288 | 2136.16 |
| int64 (Scenario 13) | 12288 | 1 (no same-address contention) | 9.29 |
| int64 (Scenario 14) | 1024 | 12 | 61.10 |
| int64 (Scenario 15) | 32 | 384 | 1120.19 |
| int64 (Scenario 16) | 1 | 12288 | 2135.92 |

The time comparison of int32 and int64 under the four contention levels is shown below (vertical axis: time; horizontal axis: left to right, contention from strong to weak):

<img src="figures/case4_1.png" alt="Case4 int32 vs int64 contention intensity comparison" style="width: 70%; height: auto;">

**Analysis**:

The performance data reveals four phenomena:

1. **Stronger contention leads to higher time.** Taking int32 as an example, as the number of contending threads per address increases from 1 (no contention) to 12, then to 384, the time rises from 15.54 μs to 111.30 μs, then to 2233.62 μs, indicating that time increases significantly with contention intensity.
2. **After contention reaches a certain level, time stops rising and tends to saturate.** For int32, the times at `target_count = 32` (384 threads/address) and `target_count = 1` (12288 threads/address) are close (2233.62 μs and 2136.16 μs). The number of contending threads increases 32-fold, yet the time remains basically unchanged.
3. **Under the same contention intensity, int64 takes less time than int32.** At `target_count = 32`, int64 takes 1120.19 μs, about half of int32's time (2233.62 μs). This result is inconsistent with the expectation that "narrower data types perform better."
4. **At maximum contention (`target_count = 1`), int32 and int64 take basically the same time.** Scenario 12 (int32) takes 2136.16 μs and scenario 16 (int64) takes 2135.92 μs — basically equal; the performance difference shown in phenomenon 3 no longer appears.

As described in Case 1, atomic operations on the same address can only execute serially. The smaller the `target_count`, the more contending threads per address, and the longer the queue wait, so the higher the time — this explains phenomenon 1. However, same-address contention alone cannot explain the time saturation or the performance difference between int32 and int64. To explain phenomena 2–4, we need to further understand the principle of atomic operations.

**Principle: GM atomic operations work at Sector granularity.**

When an atomic add accesses a GM address, it passes through the L2 cache hierarchy and is processed by the atomic processing unit on the L2 side. The L2 cache manages memory in units of 512B Cache Lines, each containing 4 contiguous 128B Sectors; GM atomic operations are processed at 128B Sector granularity. When the target addresses of atomic operations are concentrated within the same Sector, processing efficiency is lower; when the target addresses are spread across more Sectors, processing efficiency is higher.

This principle explains phenomenon 2. Taking int32 as an example, a 128B Sector can hold 32 int32 addresses (32 × 4B = 128B). At `target_count = 32`, the 32 addresses all fall within the same Sector; at `target_count = 1`, all threads concentrate on a single address, also within one Sector. Since GM atomic operations are processed at 128B Sector granularity, the target addresses in both cases fall within the same Sector, so processing efficiency is similar. Accordingly, from `target_count = 32` to `target_count = 1`, the time does not continue to increase but tends to saturate (2233.62 μs and 2136.16 μs are basically level).

Building on this principle, let us further examine the impact of data type. Since GM atomic operations are processed at 128B Sector granularity, the data-type width affects how many Sectors a given number of contiguous addresses covers. For the same number of addresses, int64 covers more Sectors, so the target addresses are more spread out and processing efficiency is higher.

The number of addresses a 128B Sector can hold depends on the width of a single address:

| Data type | Per-address size | Addresses per 128B Sector |
|:---:|:---:|:---:|
| int32 | 4B | 128B / 4B = 32 |
| int64 | 8B | 128B / 8B = 16 |

This characteristic explains phenomena 3 and 4:

- **Phenomenon 3 (int64 is faster at `target_count = 32`)**: Since GM atomic operations are processed at 128B Sector granularity, 32 int32 addresses fall within one Sector, while the same number of int64 addresses span 2 Sectors. int64 covers more Sectors, so processing efficiency is higher and the time is shorter.
- **Phenomenon 4 (both are equal at `target_count = 1`)**: Since GM atomic operations are processed at 128B Sector granularity, when all threads access the same address, regardless of whether int32 or int64 is used, the target address falls within a single Sector, so processing efficiency is similar and the times are basically equal (2136.16 μs and 2135.92 μs).

The above results show that the performance difference here is unrelated to the data-type width itself; it depends solely on the distribution density of atomic-operation addresses across 128B Sectors.


**Conclusion**: Atomic-add performance is highly sensitive to same-address contention intensity and to the distribution of target addresses within 128B Sectors. The stronger the contention and the more concentrated the target addresses, the lower the processing efficiency and the higher the time. Target addresses of atomic updates should be spread out as much as possible.

---

### Case 5: Data-type trade-off

**Goal**: On GM under a scenario with same-address contention, compare int32 and int64 atomic-add performance, showing that the choice of data type depends on whether the return value is used — one should not judge based solely on the experience that "narrower data types perform better."

**Scenario configuration**:

| Scenario | Memory | Data Type | Return Value | Scale (Block×Thread) | Atomic-add target address count |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 17 | Global Memory | int32_t | No | 48×256 | 1024 |
| 18 | Global Memory | int64_t | No | 48×256 | 1024 |
| 19 | Global Memory | int32_t | Yes | 48×256 | 1024 |
| 20 | Global Memory | int64_t | Yes | 48×256 | 1024 |

> [!NOTE]
> Since int64 atomic add only supports GM, the data-type comparison in this sample is conducted only in the GM scenario.

**Core implementation**: Launch 48 thread blocks of 256 threads each (12288 threads in total). Each thread performs one atomic add of 1, writing the result to a region in GM consisting of several contiguous addresses. The `target_count` controls the number of target addresses in GM participating in the atomic add; this group fixes `target_count = 1024`, i.e., 12 threads contend for the same address. Under the same address contention level, the four combinations of `int32_t` and `int64_t` with and without return value are tested.

```cpp
// Without return value
__global__ void atomic_add_gm_dense_i32_no_return(int32_t* counters, uint64_t target_count)
{
    asc_atomic_add(counters + ((blockIdx.x * blockDim.x + threadIdx.x) % target_count), 1);
}

// With return value
__global__ void atomic_add_gm_dense_i32_return(int32_t* counters, uint64_t target_count)
{
    volatile int32_t old_value_sink =
        asc_atomic_add(counters + ((blockIdx.x * blockDim.x + threadIdx.x) % target_count), 1);
    (void)old_value_sink;
}
```

**Performance data**:

| Return value | int32 (μs) | int64 (μs) | Performance comparison |
|:---:|---:|---:|:---:|
| Without return value (Scenario 17, 18) | 1.89 | 58.46 | int32 takes less time, about 1/31 of int64 |
| With return value (Scenario 19, 20) | 110.95 | 54.28 | int32 takes more time, about 2× of int64 |

**Analysis**:

The performance relationship between int32 and int64 is reversed in the two cases: without the return value, int32 and int64 take 1.89 μs and 58.46 μs respectively; with the return value, int32 and int64 take 110.95 μs and 54.28 μs respectively. The two cases are dominated by different mechanisms:

- **Without the return value**, int32 atomic add executes an optimized instruction. Under a scenario where about 12 threads contend for the same address, int32 and int64 take 1.89 μs and 58.46 μs respectively, indicating that int32's processing efficiency is significantly higher than int64's.
- **With the return value**, int32 cannot trigger the above optimization. Since GM atomic operations are processed at 128B Sector granularity, one Sector can hold 32 int32 addresses but only 16 int64 addresses. For the same number of addresses, int32 covers fewer Sectors, so the target addresses are more concentrated and processing efficiency is lower than int64, with its time being roughly 2× that of int64. See Case 4 for a detailed explanation of the 128B processing granularity.

The above results show that the performance relationship between int32 and int64 is not determined solely by data-type width, but also depends on whether the return value is used.

**Conclusion**: The data type should be chosen based on whether the return value is used. For pure-counting scenarios that do not use the return value, prefer int32, for which the compiler can generate an optimized instruction; in scenarios that use the return value and have densely packed target addresses, int32's target addresses are more concentrated within 128B Sectors, resulting in longer time than int64. In this case, padding or struct alignment can be used to spread out the target addresses and improve processing efficiency.

---

## Performance Comparison Summary

**All-scenario Task Duration summary**:

| Scenario | Case | Memory | Data Type | Return Value | Scale | Atomic-add target address count | Task Duration (μs) |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|---:|
| 1 | Case 1 | GM |   int32 | No | 1×1024 | 1 | 87.60 |
| 2 | Case 1 | UB |  int32 | No | 1×1024 | 1 | 1.84 |
| 3 | Case 2 | GM |   int32 | No | 8×1024 | 1 | 351.47 |
| 4 | Case 2 | UB |  int32 | No | 8×1024 | 1 | 2.25 |
| 5 | Case 3 | GM |   int32 | Yes | 1×2048 | 1 | 421.59 |
| 6 | Case 3 | GM |   int32 | No | 1×2048 | 1 | 159.88 |
| 7 | Case 3 | UB |  int32 | Yes | 1×2048 | 1 | 3.17 |
| 8 | Case 3 | UB |  int32 | No | 1×2048 | 1 | 2.76 |
| 9 | Case 4 | GM |   int32 | Yes | 48×256 | 12288 | 15.54 |
| 10 | Case 4 | GM |   int32 | Yes | 48×256 | 1024 | 111.30 |
| 11 | Case 4 | GM |   int32 | Yes | 48×256 | 32 | 2233.62 |
| 12 | Case 4 | GM |   int32 | Yes | 48×256 | 1 | 2136.16 |
| 13 | Case 4 | GM |   int64 | Yes | 48×256 | 12288 | 9.29 |
| 14 | Case 4 | GM |   int64 | Yes | 48×256 | 1024 | 61.10 |
| 15 | Case 4 | GM |   int64 | Yes | 48×256 | 32 | 1120.19 |
| 16 | Case 4 | GM |   int64 | Yes | 48×256 | 1 | 2135.92 |
| 17 | Case 5 | GM |   int32 | No | 48×256 | 1024 | 1.89 |
| 18 | Case 5 | GM |   int64 | No | 48×256 | 1024 | 58.46 |
| 19 | Case 5 | GM |   int32 | Yes | 48×256 | 1024 | 110.95 |
| 20 | Case 5 | GM |  int64 | Yes | 48×256 | 1024 | 54.28 |


## Tuning Advice

1. **Prefer UB over GM for atomic accumulation**: Atomic accumulations that can be done on UB should avoid being done on GM; when multiple thread blocks accumulate to the same address, first complete the accumulation in each block's UB, then have each block write back to GM once, rather than having all threads directly perform atomic adds on GM.
2. **Spread out the target addresses of atomic operations**: GM atomic operations use the 128B Sector as the hardware processing granularity. When target addresses are concentrated within the same Sector, processing efficiency is lower; when they are spread across more Sectors, processing efficiency is higher. Therefore, different threads should access target addresses that are as spread out as possible.
3. **Do not use the return value unless necessary**: Without the return value, the compiler generates a better-performing instruction; for pure-counting scenarios, prefer int32 (which has an instruction optimization when the return value is unused). If the business logic requires the return value and target addresses are densely packed, consider int64 or apply padding to int32.

## Build and Run

Execute the following steps in the sample root directory to build and run the sample.

- Configure environment variables

  Configure the environment variables according to the [installation method](../../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit on your current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN installation directory. When no installation directory is specified, it is installed under `/usr/local/Ascend` by default.

- Run the sample

  Build and run scenario 1:

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1 ..
  make -j
  ./atomic_add_perf
  ```

  Build options:

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this sample supports only dav-3510 (Ascend 950PR/Ascend 950DT) |
  | `SCENARIO_NUM` | `1`-`20` | Scenario number to test. The scale (Block×Thread), data type, whether the return value is used, and `target_count` for each scenario are configured centrally by `get_scenario_config()` in the source |
  | `SKIP_VALIDATION` | `ON`/`OFF` | Whether to skip result validation; default `OFF`. The UB-only scenarios of Case 1/3 have no GM result and automatically skip validation; recommended to set to `ON` when profiling GM-writeback scenarios with `msopprof` |

  The following output indicates that correctness validation passed.

  ```text
  Scenario 1: Case1 GM int32 single-address, no return value
  Workload: blocks=1 threads_per_block=1024 total_atomic_adds=1024
  Validation passed
  ```

## Performance Analysis

Use the `msopprof` tool to obtain detailed performance data:

```bash
msopprof ./atomic_add_perf
```

After the command completes, a folder named `OPPROF_{timestamp}_XXX` is generated in the default directory. An example of the performance-data folder structure is as follows:

```text
├──dump                       # Raw performance data; users can ignore this
├──ArithmeticUtilization.csv  # cube/vector instruction cycle share
├──L2Cache.csv                # L2 Cache hit rate
├──Memory.csv                 # UB, L1, and main memory read/write bandwidth
├──MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth
├──MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth
├──OpBasicInfo.csv            # Operator basic information
├──PipeUtilization.csv        # Compute and move (DMA) unit time and share
├──ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio across all instructions
└──visualize_data.bin         # MindStudio Insight presentation file
```

> [!NOTE]
> **About Validation failed during profiling**: Apart from the UB-only scenarios of Case 1/3, the final accumulation result of each sample scenario is written back to a GM counter, which is zeroed only once when allocated on the host side; the kernel only accumulates and does not zero it on entry. `msopprof`'s warmup+replay executes the kernel repeatedly on the same GM memory, so the counter is accumulated multiple times. Under strict validation this reports `Validation failed` (the count is an integer multiple of the expected value).
>
> Although the UB intra-block accumulation scenario of Case 2 zeroes the intermediate UB accumulator on kernel entry (making it idempotent under replay), the block subtotal is still written back to the GM counter via `asc_atomic_add`. That GM counter is likewise accumulated multiple times under replay, so validation fails for it too.
>
> This is an inherent conflict between the replay mechanism and the validation logic. When profiling any scenario, it is recommended to skip validation with `-DSKIP_VALIDATION=ON`.

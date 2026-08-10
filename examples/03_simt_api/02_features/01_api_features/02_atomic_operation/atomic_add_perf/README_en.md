# asc_atomic_add Interface Performance Comparison Sample

## Overview

Taking the [asc_atomic_add()](../../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md) interface as an example, this sample uses five groups of controlled experiments to demonstrate the performance differences of `asc_atomic_add()` under various usage scenarios — including different memory tiers (Global Memory / Unified Buffer), different atomic-accumulation organizations (direct atomic add / intra-block UB accumulation followed by GM writeback), whether the return value is used, the contention intensity on the same address, the Sector distribution of target addresses, and different data types (int32_t, int64_t) — and provides corresponding optimization guidance.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Versions

- \>= CANN 9.1.0

## Directory Structure

```text
├── atomic_add_perf
│   ├── figures                         // Image resources for README
│   ├── CMakeLists.txt                  // Sample build script
│   ├── atomic_add_perf.asc             // Sample source code
│   ├── README.md
│   └── README_en.md
```

## Sample Description

This sample contains 16 scenarios in total, testing the performance of `asc_atomic_add()` under the following usage conditions:

- Different memory tiers (Global Memory, Unified Buffer).
- Different atomic-accumulation organizations (direct atomic add, intra-block UB accumulation followed by GM writeback).
- Whether the return value is used.
- Contention intensity on the same address and the Sector distribution of target addresses.
- Data type (int32_t, int64_t).

The 16 scenarios are grouped into 5 controlled experiments (Case 1~5) for performance comparison, ultimately providing the performance characteristics of `asc_atomic_add()`. The sample selects one scenario to test via `SCENARIO_NUM`; each run launches only one target kernel.

**Five controlled groups**:

| Case | Dimension | Scenarios |
|:---:|---|---|
| Case 1 | Memory tier under a single thread block: Global Memory → Unified Buffer | Scenario 1 ↔ Scenario 2 |
| Case 2 | Memory tier under multiple thread blocks: Global Memory → Unified Buffer | Scenario 3 ↔ Scenario 4 |
| Case 3 | Return-value usage: with return value → without return value | Scenario 5 ↔ Scenario 6 (GM)<br>Scenario 7 ↔ Scenario 8 (UB) |
| Case 4 | Increasing the number of threads per address under Padding layout; comparing Padding vs. dense layout with the same workload | Scenario 9→10→11, Scenario 11↔Scenario 12 |
| Case 5 | Data-type trade-off: int32_t → int64_t | Scenario 13 ↔ Scenario 14 (no return value)<br>Scenario 15 ↔ Scenario 16 (with return value) |

## Sample Implementation

This section analyzes the impact of memory tier, atomic organization, return value, same-address contention intensity, target-address Sector distribution, and data-type trade-off on atomic-add performance group by group (Case 1~5). Each group changes only one attribute while keeping the others identical, and provides the corresponding `msopprof` measured data and root-cause analysis.

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

**Core implementation**: Launch 1 thread block with 1024 threads per block. In the GM scenario, 1024 threads perform an atomic add on the same GM address; in the UB scenario, 1024 threads perform an atomic add on the same UB address. Case 1 only compares the overhead of the interface accessing GM versus UB; the UB scenario does not perform correctness validation.

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

**Core implementation**: Launch 8 thread blocks of 1024 threads each (8192 threads in total). In the GM scenario, 8192 threads directly perform an atomic add on the same GM address; in the UB scenario, each block first accumulates on its own UB array `local[1]`, then each block performs one GM atomic-add writeback.

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

### Case 4: Contention intensity on the same address and the impact of Sector distribution

**Goal**: Compare the impact on atomic-add performance of different thread counts per address (contention intensity) and the Sector distribution of target addresses.

To understand how these two factors affect atomic-add performance, the following principles are introduced first.

**Principle 1: Atomic operations on the same address queue serially.**

An atomic add must complete three steps: "read old value → compute new value → write back new value," guaranteeing the entire process is indivisible. When multiple threads update the same address, atomic operations can only complete one at a time; the more threads on the same address, the more atomic adds waiting to execute, and the higher the total time.

**Principle 2: GM atomic operations work at Sector granularity.**

When an atomic add accesses a GM address, it passes through the L2 cache hierarchy and completes the "read–compute–write" on the L2 side. The L2 cache manages memory in units of 512B Cache Lines, each containing 4 contiguous 128B Sectors; GM atomic operations are processed at 128B Sector granularity. When the target addresses of atomic operations are concentrated within the same Sector, processing efficiency is lower; when the target addresses are spread across more Sectors, processing efficiency is higher.

**Controlled design**:

1. **Contention-intensity comparison (Scenario 9→10→11)**: Using Padding layout to ensure that each target address resides in an independent Sector, increase the number of threads per address from 1 to 12 and 384. Each active thread performs 1 atomic add, so the total number of atomic adds increases in proportion to the thread count.
2. **Sector-distribution comparison (Scenario 11↔Scenario 12)**: Keep the number of target addresses, threads per address, and total atomic adds the same; only change the address layout from Padding to dense so that multiple target addresses share one Sector.

**Scenario configuration**: All scenarios use 32 GM target addresses and all use the atomic-add return value; each active thread performs only 1 atomic add.

| Scenario | Memory | Data Type | Scale (Block×Thread) | Address layout | Threads per address | Total atomic adds | Target-address Sectors covered |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 9 | Global Memory | int32_t | 48×256 | Padding | 1 | 32 | 32 |
| 10 | Global Memory | int32_t | 48×256 | Padding | 12 | 384 | 32 |
| 11 | Global Memory | int32_t | 48×256 | Padding | 384 | 12288 | 32 |
| 12 | Global Memory | int32_t | 48×256 | Dense | 384 | 12288 | 1 |

**Address layout illustration**: A 128B Sector can hold 32 `int32_t` addresses. Padding layout places 32 target addresses in 32 separate Sectors; dense layout places 32 target addresses contiguously within a single Sector.

The two address layouts are compared in the following figure:

<img src="figures/case4_address_layout.png" alt="Case 4 Padding and dense layout comparison" style="width: 90%; height: auto;">

**Scenario logic**: All four scenarios launch 48 thread blocks with 256 threads per block. Each active thread performs one atomic add on one target address.

- **Scenario 9**: Enables 32 threads to operate on 32 target addresses in Padding layout, with 1 thread per address. The 32 addresses are distributed across 32 Sectors.
- **Scenario 10**: Enables 384 threads to operate on 32 target addresses in Padding layout, with 12 threads per address. The 32 addresses are distributed across 32 Sectors.
- **Scenario 11**: Enables 12288 threads to operate on 32 target addresses in Padding layout, with 384 threads per address. The 32 addresses are distributed across 32 Sectors.
- **Scenario 12**: Enables 12288 threads to operate on 32 densely packed target addresses, with 384 threads per address. The 32 addresses are concentrated in 1 Sector.

> [!NOTE]
> This group of scenarios uniformly uses the return value: as shown by Case 3, when the return value is not used, int32 triggers an instruction optimization whose time is dominated by that optimization, masking the effects of same-address thread count and Sector distribution on performance.

```cpp
__global__ void atomic_add_gm_case4_i32_return(
    int32_t* counters, uint64_t target_count, uint64_t threads_per_target,
    uint64_t target_stride)
{
    uint64_t global_tid = blockIdx.x * blockDim.x + threadIdx.x;
    uint64_t active_threads = target_count * threads_per_target;
    if (global_tid >= active_threads) {
        return;
    }

    uint64_t target_id = global_tid % target_count;
    int32_t* target = counters + target_id * target_stride;
    volatile int32_t old_value_sink = asc_atomic_add(target, 1);
    (void)old_value_sink;
}
```

**Performance data**:

| Scenario | Task Duration (μs) |
|:---:|---:|
| 9 | 2.53 |
| 10 | 5.11 |
| 11 | 80.17 |
| 12 | 2164.72 |

**Analysis**:

1. **Under Padding layout, as the number of threads per address and total atomic adds increase, total time increases monotonically.** When threads per address increase from 1 to 12 and 384, time increases from 2.53 μs to 5.11 μs and 80.17 μs. This is because each thread performs 1 atomic add, so increasing the thread count increases the total number of atomic adds proportionally, and atomic operations on the same address must queue serially — the more atomic adds waiting, the higher the time.
2. **Under the same workload, densely packed target addresses significantly increase time.** Compared to the Padding scenario 11, the dense-layout scenario 12 increases time from 80.17 μs to 2164.72 μs — roughly 27.0×. This is because Padding layout spreads 32 target addresses across 32 Sectors, while dense layout concentrates them in 1 Sector; GM atomic operations are processed at 128B Sector granularity, and processing efficiency is lower when target addresses are concentrated.

**Conclusion**:

- The more threads performing atomic operations on the same address, the higher the overall time.
- When atomic operations must be performed on GM, contention intensity should be evaluated at Sector granularity. When the business scenario allows, Padding can be used to spread out target addresses and reduce time.

---

### Case 5: Data-type trade-off

**Goal**: On GM under a same-address-contention scenario, compare int32 and int64 atomic-add performance, showing that the choice of data type depends on whether the return value is used — one should not judge based solely on the experience that "narrower data types perform better."

**Scenario configuration**:

| Scenario | Memory | Data Type | Return Value | Scale (Block×Thread) | Atomic-add target address count |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 13 | Global Memory | int32_t | No | 48×256 | 1024 |
| 14 | Global Memory | int64_t | No | 48×256 | 1024 |
| 15 | Global Memory | int32_t | Yes | 48×256 | 1024 |
| 16 | Global Memory | int64_t | Yes | 48×256 | 1024 |

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
| Without return value (Scenario 13, 14) | 1.89 | 58.46 | int32 takes less time, about 1/31 of int64 |
| With return value (Scenario 15, 16) | 110.95 | 54.28 | int32 takes more time, about 2× of int64 |

**Analysis**:

The performance relationship between int32 and int64 is reversed in the two cases: without the return value, int32 and int64 take 1.89 μs and 58.46 μs respectively; with the return value, int32 and int64 take 110.95 μs and 54.28 μs respectively. The two cases are dominated by different mechanisms:

- **Without the return value**, int32 atomic add executes an optimized instruction. Under a scenario where about 12 threads contend for the same address, int32 and int64 take 1.89 μs and 58.46 μs respectively, indicating that int32's processing efficiency is significantly higher than int64's.
- **With the return value**, int32 cannot trigger the above optimization, and the measured time is roughly 2× that of int64. When target addresses are densely packed, int32 and int64 cover different numbers of Sectors; additionally, changing the data type also changes the data width of the atomic instruction, so this cross-type data is only used for comparing actual performance and should not be taken as independent evidence of Sector impact. Case 4 demonstrates the impact of Sector distribution through a Padding-vs-dense comparison within the same type.

The above results show that the performance relationship between int32 and int64 is not determined solely by data-type width, but also depends on whether the return value is used.

**Conclusion**: The data type should be chosen based on whether the return value is used. For pure-counting scenarios that do not use the return value, prefer int32, for which the compiler can generate an optimized instruction; in scenarios that use the return value, the data type should be selected based on measured results. When target addresses are densely packed, padding or struct alignment can be used to spread out the target addresses.

---

## Performance Comparison Summary

**All-scenario Task Duration summary**:

| Scenario | Case | Memory | Data Type | Return Value | Scale | Atomic-add config | Task Duration (μs) |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|---:|
| 1 | Case 1 | GM |   int32 | No | 1×1024 | 1 address | 87.60 |
| 2 | Case 1 | UB |  int32 | No | 1×1024 | 1 address | 1.84 |
| 3 | Case 2 | GM |   int32 | No | 8×1024 | 1 address | 351.47 |
| 4 | Case 2 | UB |  int32 | No | 8×1024 | 1 address | 2.25 |
| 5 | Case 3 | GM |   int32 | Yes | 1×2048 | 1 address | 421.59 |
| 6 | Case 3 | GM |   int32 | No | 1×2048 | 1 address | 159.88 |
| 7 | Case 3 | UB |  int32 | Yes | 1×2048 | 1 address | 3.17 |
| 8 | Case 3 | UB |  int32 | No | 1×2048 | 1 address | 2.76 |
| 9 | Case 4 | GM |   int32_t | Yes | 48×256 | 32 Padding addresses, 1 thread/address, 1 add/thread | 2.53 |
| 10 | Case 4 | GM |   int32_t | Yes | 48×256 | 32 Padding addresses, 12 threads/address, 1 add/thread | 5.11 |
| 11 | Case 4 | GM |   int32_t | Yes | 48×256 | 32 Padding addresses, 384 threads/address, 1 add/thread | 80.17 |
| 12 | Case 4 | GM |   int32_t | Yes | 48×256 | 32 dense addresses, 384 threads/address, 1 add/thread | 2164.72 |
| 13 | Case 5 | GM |   int32 | No | 48×256 | 1024 dense addresses | 1.89 |
| 14 | Case 5 | GM |   int64 | No | 48×256 | 1024 dense addresses | 58.46 |
| 15 | Case 5 | GM |   int32 | Yes | 48×256 | 1024 dense addresses | 110.95 |
| 16 | Case 5 | GM |  int64 | Yes | 48×256 | 1024 dense addresses | 54.28 |


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
  | `SCENARIO_NUM` | `1`-`16` | Scenario number to test. The scale (Block×Thread), data type, whether the return value is used, `target_count`, `threads_per_target`, and `target_stride` for each scenario are configured centrally by `get_scenario_config()` in the source |
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

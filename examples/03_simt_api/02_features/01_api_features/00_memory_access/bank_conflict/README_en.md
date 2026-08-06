# SIMT UB Bank Conflict Example

## Overview

This example introduces UB bank conflicts that may occur when multiple threads in the same Warp access UB in the Ascend C SIMT programming model. The example constructs 5 cases with different UB read address patterns, demonstrating typical scenarios such as same-address merge, cross-bank/subbank distributed access, same 8B-range merge for contiguous float access, same-subbank different-row conflicts, and same-index subbank conflicts within the same bank group.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```text
├── bank_conflict
│   ├── figures              // Image resources for README
│   ├── bank_conflict.asc    // SIMT UB Bank conflict example implementation
│   ├── CMakeLists.txt       // CMake build file
│   ├── README.md            // Example documentation
│   └── README_en.md         // Example documentation
```

## Example Description

### Example Function

This example is used to verify UB bank conflict behavior when multiple threads in the same Warp access UB in Ascend C SIMT programming. The example uses the compile-time parameter `SCENARIO_NUM` to select different cases and construct typical access patterns, including same-address merge, cross-subbank distributed access, same 8B-range merge in contiguous float access, same-subbank different-row conflicts, and same-index subbank conflicts within the same bank group.

On the Host side, the input array is constructed and copied to GM. The kernel first moves input data from GM to UB, then repeatedly reads data according to the UB address pattern of the current case, and uses [`clock()`](../../../../../../docs/zh/api/Utils-API/tuning_interface/clock.md) to count the cycles spent reading UB data.

### Example Specifications

<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">BankConflict</td></tr>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">input</td><td align="center">[4096]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Example Output</td><td align="center">output</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">cycle_output</td><td align="center">[32]</td><td align="center">uint64_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">bank_conflict_kernel</td></tr>
</table>

## Example Implementation

### UB Bank and Subbank Rules

In this document, UB bank conflict in SIMT programming mainly refers to a subbank conflict where multiple threads in the same Warp compete for same-index subbank resources in the same bank group in one UB access instruction.

Taking Ascend 950PR/Ascend 950DT as an example, the following figure shows the UB bank structure. The total UB size is 256KB and is divided into 8 bank groups. Each bank group contains 2 banks. The 16 banks are numbered from bank0 to bank15. Bank i and bank i+8 belong to bank group i, namely `bank_group_number = bank_number % 8`. Each bank is 16KB and contains 512 rows, with each row being 32B. In SIMT programming mode, each 32B row in a bank is further divided into 4 subbanks at 8B granularity.

In SIMT scenarios, multiple threads in the same Warp may access UB at the same time in one UB access instruction. When the accessed data belongs to subbank resources with the same index in the same bank group, for example, subbank2 in bank0 and subbank2 in bank8, the hardware cannot process all requests in one cycle, so the requests need to wait in sequence, forming subbank conflicts and increasing access latency. If the accessed data belongs to the same 8B address range in the same row of the same subbank, the hardware merges these requests, and no subbank conflict is formed.

**Figure 1** Bank structure
![](./figures/bank结构示意图.png)

### Address Encoding Rules

UB addresses use low-bit interleaving. As shown in the following figure, consecutive addresses are mapped to bank0 through bank15 at 32B granularity: the first 32B address segment maps to bank0, the second maps to bank1, and so on. The 16th segment maps to bank15, and the 17th segment maps back to the next row of bank0.

**Figure 2** Bank memory layout
![](./figures/bank内存排布示意图.png)

The main subbank conflict types in SIMT programming are as follows:

- **Write-Write Conflict**: Multiple write operations simultaneously attempt to access subbanks with the same index in the same bank group, for example, multiple threads simultaneously write subbank2 in bank0 and subbank2 in bank8.
- **Read-Read Conflict**: Multiple read operations simultaneously attempt to access subbanks with the same index in the same bank group, for example, multiple threads simultaneously read subbank2 in bank0 and subbank2 in bank8.

In SIMT programming mode, threads in the same Warp execute the same instruction at the same time, so there is no read-write conflict scenario. This example focuses on read-read conflicts. It uses multiple threads in the same UB read instruction to access different UB address patterns and observe the cycle differences among mergeable access, conflict-free access, and subbank-conflict access.

### Case Implementation Description

This example uses a `float` array as the UB access object. Each thread computes one UB read index according to its `thread_id`. Each case only changes how `ub_index` is computed in `get_ub_index_device()`, so that 32 threads form different access distributions in the same UB read instruction: exactly the same address, 8B-granularity distribution to different subbanks, contiguous addresses, 512B-stride access returning to different rows of the same bank and subbank, or 256B-stride access landing on same-index subbank resources in the same bank group.

With this design, the example compares the access merge and subbank conflict differences caused only by UB address pattern changes while keeping the data type and thread organization unchanged.

The kernel first moves input data from GM to the `__ubuf__` array `ub`, obtains the current thread ID through `threadIdx.x`, and calls `get_ub_index_device()` to obtain the UB read index for the current case. `ub_ptr` points to the same UB array. The `volatile` qualifier is added to ensure that each read in the loop remains an actual UB read access, preventing the compiler from optimizing repeated reads into a single read.

```cpp
for (uint32_t i = tid; i < UB_ELEMENT_COUNT; i += blockDim.x) {
    ub[i] = input[i];
}

uint32_t ub_index = get_ub_index_device(tid);
__ubuf__ volatile float* ub_ptr;
ub_ptr = ub;

#pragma unroll
for (uint32_t i = 0; i < REPEAT_TIMES; ++i) {
    acc += ub_ptr[ub_index];
}
```

**Table 2** Case implementation description

| Case   | Scenario Description                                                 | Expected Access Characteristics                                                                |
| ------ | -------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------- |
| Case 0 | Threads in the same Warp read the same address                       | Same-address requests are merged                                                               |
| Case 1 | Cross-subbank access                                                 | 32 threads access different subbanks across 8 bank groups; this is the conflict-free baseline  |
| Case 2 | Threads in the same Warp access adjacent float elements continuously | Adjacent two threads access the same 8B range, and the hardware can merge the requests         |
| Case 3 | Access different rows of the same subbank in the same bank           | 32 threads access different rows of the same bank and same subbank, forming a subbank conflict |
| Case 4 | Access same-index subbanks in the same bank group                    | 32 threads access same-index subbanks in the same bank group, forming a subbank conflict       |

### Data Partitioning and Thread Block Layout

The kernel entry of this example is marked with `__global__`, and the maximum thread count is specified through `__launch_bounds__`. The example launches 1 Thread Block, and each Thread Block contains 32 threads, corresponding to one Warp. The example does not partition data across multiple blocks. Instead, it uses the 32 threads in one Warp to construct different UB access patterns and observe access merge and subbank conflict differences in the same UB access instruction.

- `gridDim`: `(1, 1, 1)`.
- `blockDim`: `(32, 1, 1)`.
- The range of `threadIdx.x` is 0-31, corresponding to 32 threads in the same Warp.
- Each thread first participates in GM-to-UB movement with the index pattern `threadIdx.x + n * blockDim.x`, where `n` indicates the movement loop iteration, so that `input[4096]` is fully moved into the UB array.
- This example launches only 32 threads, and these 32 threads are in the same Warp. After GM-to-UB movement, each thread directly computes the UB read index according to its `thread_id` and the current case, and repeatedly reads the data `REPEAT_TIMES` times.
- The Host side verifies that the accumulated output of the 32 threads matches the CPU result. The cycles of different cases can be used to compare the overhead caused by different UB access patterns.

### Access Timing

Because subbank conflicts are lower-level storage conflicts that are not captured by the operator tuning tool msOpProf, this example uses `clock()` to time the UB access code. The difference between the loop start and end timestamps is used as the cycles spent by the current thread reading UB data and is written to `cycle_output` for return to the Host side. The timing code is as follows:

```cpp
uint64_t start = clock();
// Repeated UB read loop.
uint64_t end = clock();
cycle_output[threadIdx.x] = end - start;
```

### Performance Metrics Description

The kernel writes the cycles counted by each of the 32 threads to `cycle_output[32]`. The Host side prints the cycles of the 32 threads in thread order to compare access overhead between different cases.

The output format is as follows:

```text
Cycles per thread: <thread0> <thread1> ... <thread31>
```

### Case 0: Threads in the Same Warp Read the Same Address

**Example Goal**: Demonstrate that when multiple threads in the same Warp read exactly the same UB address, the hardware can merge same-address requests.

**Core Implementation**:

```cpp
__aicore__ inline uint32_t get_ub_index_device(uint32_t thread_id)
{
...
#if SCENARIO_NUM == 0
    // All threads read exactly the same UB address.
    return 0;
...
}
```

All 32 threads read `ub[0]`. These accesses have exactly the same start address and belong to a same-address read scenario. The hardware can merge them, so no subbank conflict is generated. The access pattern is shown below:

![](./figures/case0.png)

**Performance Data**:

```text
Cycles per thread: 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078
```

**Analysis**:

In Case 0, the read requests of 32 threads point to the same UB address. The hardware can merge these requests, so no subbank resource contention is generated.

### Case 1: Cross-Bank/Subbank Distributed Access

**Example Goal**: Construct a conflict-free UB access baseline.

**Core Implementation**:

```cpp
__aicore__ inline uint32_t get_ub_index_device(uint32_t thread_id)
{
...
#elif SCENARIO_NUM == 1
    // 8B access stride distributes 32 threads to 4 subbanks in 8 bank groups.
    return thread_id * 2;
...
}
```

The accesses of 32 threads cover a continuous 256B region and are distributed to 4 subbanks in 8 bank groups at 8B granularity. Under the same UB access instruction, each subbank in each bank group is accessed by only one thread. Therefore, this case can be used as a conflict-free access baseline. The access pattern is shown below:

![](./figures/case1.png)

**Performance Data**:

```text
Cycles per thread: 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107
```

**Analysis**:

In Case 1, the 32 threads access 4 subbanks in 8 bank groups at 8B granularity. Under the same UB access instruction, each subbank in each bank group is accessed by only one thread, so no subbank resource contention is formed. This case is used as the conflict-free baseline for comparison with Case 2, Case 3, and Case 4.

### Case 2: Same 8B-Range Merge in Contiguous Float Access

**Example Goal**: 32 threads access 32 different addresses. Case 2 accesses adjacent float elements continuously, so every adjacent two threads access the same 8B range, and the hardware can merge the requests.

**Core Implementation**:

```cpp
__aicore__ inline uint32_t get_ub_index_device(uint32_t thread_id)
{
...
#elif SCENARIO_NUM == 2
    // Contiguous float access makes every adjacent two threads fall into the same 8B subbank range.
    return thread_id;
...
}
```

In this case, the 32 threads read `ub[0]` through `ub[31]`. For the float type, `ub[0]` and `ub[1]` are in the same 8B range in the same row of the same subbank, `ub[2]` and `ub[3]` are in the same 8B range in the same row of the next subbank, and so on. The accesses of every adjacent two threads can be merged according to the same 8B range. The access pattern is shown below:

![](./figures/case2.png)

**Performance Data**:

```text
Cycles per thread: 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081
```

**Analysis**:

In Case 2, the 32 threads access contiguous `float` elements. Every adjacent two threads access the same 8B range, and the hardware can merge these requests. Compared with Case 1, both cases access 32 different addresses, but Case 2 has same-8B-range merge between adjacent threads, so its cycles are slightly lower than Case 1.

### Case 3: Same-Subbank Different-Row Conflict in the Same Bank

**Example Goal**: Construct 32 threads that access different addresses but all concentrate on different rows of the same subbank.

**Core Implementation**:

```cpp
__aicore__ inline uint32_t get_ub_index_device(uint32_t thread_id)
{
...
#elif SCENARIO_NUM == 3
    // 512B access stride returns addresses to different rows of the same bank and same subbank.
    return thread_id * 128;
...
}
```

According to the UB low-bit interleaving rule, a 512B access stride makes the addresses return to the same bank group, same bank, and same subbank, while the depth row ID increments. Therefore, the 32 threads in the same Warp simultaneously access different rows of the same-index subbank in the same bank, forming a subbank conflict. The access pattern is shown below:

![](./figures/case3.png)

**Performance Data**:

```text
Cycles per thread: 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760
```

**Analysis**:

In Case 3, 32 threads access different rows of the same bank and same subbank, causing contention for the same subbank resource. Compared with the conflict-free baseline in Case 1, cycles increase significantly, indicating that this access pattern forms a subbank conflict.

### Case 4: Same-Index Subbank Conflict in the Same Bank Group

**Example Goal**: Construct 32 threads that access different addresses but all concentrate on same-index subbank resources in the same bank group.

**Core Implementation**:

```cpp
__aicore__ inline uint32_t get_ub_index_device(uint32_t thread_id)
{
...
#elif SCENARIO_NUM == 4
    // 256B access stride switches the bank ID inside the bank group but keeps the subbank ID unchanged.
    return thread_id * 64;
...
}
```

`thread_id * 64` means that adjacent threads are spaced by 64 `float` elements, that is, 256B. A 256B access stride switches the bank ID inside the bank group but keeps the bank group ID and subbank ID unchanged. Therefore, the 32 threads in the same Warp concentrate on same-index subbank resources in the same bank group, forming a subbank conflict. The access pattern is shown below:

![](./figures/case4.png)

**Performance Data**:

```text
Cycles per thread: 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760
```

**Analysis**:

In Case 4, the 32 threads concentrate on same-index subbank resources in the same bank group. Compared with Case 1, cycles increase significantly, indicating that accessing same-index subbanks in the same bank group also causes subbank resource contention.

### Tuning Suggestions

Case 0 and Case 2 show that mergeable access does not form subbank conflicts, and Case 1 is the conflict-free baseline. The cycles of Case 3 and Case 4 increase significantly, indicating that concentrating accesses on same-index subbanks in the same bank group causes subbank resource contention.

When writing SIMT code, avoid concentrating multiple threads in the same Warp on same-index subbanks in the same bank group in one UB access instruction. If such an access pattern is unavoidable, consider adjusting the data layout, access stride, or adding padding in UB to distribute subbank accesses. For an optimization example that avoids access conflicts, see the [matrix transpose example](../../../../03_best_practices/00_memory_optimizations/matrix_transpose_practice).

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.

- Configure Environment Variables

  Configure environment variables based on the [installation method](../../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```
  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
  >
- Run the Example

  Run the following commands in this example directory.

  ```bash
  SCENARIO_NUM=0                       # Select the execution scenario, options 0-4
  mkdir -p build && cd build;          # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..; make -j;  # Build the project
  ./demo                               # Run the example
  ```
- Build Options Description

  | Option                      | Values       | Description                                                                       |
  | --------------------------- | ------------ | --------------------------------------------------------------------------------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |
  | `SCENARIO_NUM`            | `0`-`4`  | Example type, default value is 0                                                  |

  The following output indicates that the accuracy verification is successful.


  ```text
  [Success] Case accuracy verification passed.
  ```

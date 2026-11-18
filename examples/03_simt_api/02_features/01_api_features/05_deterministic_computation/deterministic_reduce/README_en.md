# SIMT Programming Deterministic Computation Example

## Overview

This example uses the reduction sum of a 1D `float` input to demonstrate the non-deterministic computation problem in Ascend C SIMT programming scenarios and its common solutions.

## Supported Products and CANN Versions

| Products | CANN Versions |
|---|---|
| Ascend 950PR&950DT products | \>= CANN 9.2.0 |

## Directory Structure

```text
├── deterministic_reduce
│   ├── deterministic_reduce.asc  // Triple-case reduction example: atomic non-deterministic accumulation, deterministic halving reduction tree, and deterministic butterfly reduction tree
│   ├── figures                   // Figure resources referenced by the README
│   ├── CMakeLists.txt            // Build project file
│   ├── README.md
│   └── README_en.md              // English version of the sample description
```

## Non-deterministic Computation in SIMT Scenarios

### Root Causes of Non-deterministic Computation

**Root cause 1: atomic operations do not guarantee execution order**

An atomic operation guarantees that the **single read-modify-write process is not interrupted**, but it does not guarantee the order in which multiple threads arrive at the atomic operation. When multiple threads issue atomic operations on the same address, the requests are executed one by one rather than in parallel; which thread's request runs first is decided by hardware scheduling and cannot be specified by software.

**Root cause 2: floating-point addition is not strictly associative**

For integer-counting atomic accumulation (for example, all threads executing `asc_atomic_add(counter, 1U)`), addition is associative, so the final count is the same regardless of order and the result is deterministic.

Floating point is different: floating-point addition is not strictly associative, so `((a + b) + c) + d` and `(a + b) + (c + d)` may produce different results. Every floating-point addition involves rounding under finite-precision representation; different accumulation orders produce different intermediate roundings, which affect the low bits of the final result.

**Combined: an uncontrollable order applied to an order-sensitive operation**

With both root causes combined, the final result of multi-thread floating-point atomic accumulation depends not only on the participating data but also on the **actual execution order of each thread's atomic request in the current run**. Since the execution order varies with thread scheduling:

- **Within one run**, the low-bit rounding path of the final result is decided by the scheduling order at that moment.
- **Across runs**, if the scheduling order changes, the low bits may drift.

### Common Methods for Deterministic Computation

To obtain deterministic computation in SIMT scenarios, the industry-standard idea is **to take "order" back from run-time scheduling into software**: either make the computation order independent of scheduling, or make the computation itself order-insensitive. Common methods are as follows:

- **Fixed-order layered reduction tree**: perform layered reduction through warp capabilities and UB intermediate staging; every layer's pairing order is visible in software, without floating-point atomic accumulation and without any primitive whose execution order depends on scheduling.

- **Switch to order-insensitive operations or representations**: carry intermediate results in a higher-precision data type, so that the rounding differences of the different paths are absorbed beyond the final precision.

The following sections use a reduction-sum operator as the example to show the solution for non-deterministic computation in Ascend C SIMT programming scenarios.

## Example Description

### Example Function

Accumulate a 1D `float` input and output the accumulated value.

### Example Specifications

  <table border="1" align="center">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">DeterministicReduce</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[1]</td><td align="center">float</td><td align="center">ND</td></tr>
  </table>

- Data Partitioning:

  - gridDim: (4, 1, 1)
  - blockDim: (2048, 1, 1)
  - Per-thread processing: 1 input element

## Example Implementation

### Precision Error Criterion

The Host-side golden uses the simplest sequential sum (a0 + a1 + a2 + ..., step-by-step float addition, replaying no Device reduction tree); each case runs 10 rounds, and every round's ULP error against the golden is recorded. ULP (Unit in the Last Place) is the distance in the last significant position: for two same-sign floats, the integer difference of their bit patterns is the ULP error; at this example's result magnitude (about 12.8e9), 1 ULP = 1024.

### Scenario 1: Atomic Non-deterministic Accumulation

**Core implementation**: each thread reads its own input element, and all 8192 threads execute `asc_atomic_add` on the same GM float address:

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void kernel_atomic_accumulate(const float* input, float* output)
{
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    asc_atomic_add(output, input[idx]);
}
```

**Figure 1**  Computation flow of Case 0 (taking only 4 inputs as an example, with ①②③④ marking the actual accumulation order: with the same input, two runs have different actual scheduling orders, so the accumulation order differs and result A and result B may differ)

<img src="./figures/deterministic_case0_atomic.png">

#### Precision Comparison and Analysis

The Host side reads back each round's final value, prints its binary representation (bits) and its ULP error against the sequential golden, and compares the rounds against each other to show the drift distribution.

- **Precision non-determinism**: the addition order of atomic accumulation is decided by scheduling, so the ULP error drifts across rounds, and several different values may appear within the 10 rounds.

One 10-round measurement with the current 4×2048 configuration is as follows. The golden's sequential-sum result is 0x503ebc20 (12800000000.0); at this magnitude, 1 ULP = 1024:

| Result (bit pattern) | Result (decimal) | ULP error vs golden | Occurrences in 10 rounds |
| --- | --- | --- | --- |
| 0x503ebc1e | 12799997952.0 | -2 | 1 |
| 0x503ebc1a | 12799993856.0 | -6 | 3 |
| 0x503ebc1b | 12799994880.0 | -5 | 2 |
| 0x503ebc1c | 12799995904.0 | -4 | 4 |

The ULP error drifted between -2 and -6 in this run, confirming that the floating-point atomic accumulation result depends on execution order.

### Scenario 2: Deterministic Reduction with the Halving Tree

Halving reduction is the most common in-block reduction pattern across the industry: a UB staging buffer is allocated; the N participating threads first write their own elements into the buffer, the N threads split into a front half and a rear half, and in each round the threads of the front half add the data of the rear-half threads to their own slots; the number of active threads then halves together with the stride, and after log2(N) rounds the sum of all N values is obtained.

**Figure 2**  Computation flow of the halving tree, illustrated with 16 example values

<img src="./figures/deterministic_case1_halving.png">

```cpp
smem[tid] = input[blockIdx.x * blockDim.x + tid];
asc_syncthreads();
for (uint32_t stride = blockDim.x / 2; stride > 0; stride /= 2) {
    if (tid < stride) {
        smem[tid] += smem[tid + stride];
    }
    asc_syncthreads();
}
if (tid == 0) {
    block_sums[blockIdx.x] = smem[0];
}
```

Each round follows "sync after write-back, then read", guaranteeing that within a round the whole group writes first and reads later, with no overlapping read/write. After the halving completes, an inter-block fixed-order merge runs: only thread 0 writes the block sum. Thread 0 of the last-completed block accumulates the 4 block sums in order and writes the final result.

The halving tree fixes the addition pairing order of the elements; the computation process is straightforward, and it is the most common reduction pattern in the ecosystem. However, only the front half of the threads stay active in each round, and every round reads and writes data in the UB staging buffer, so its performance is not optimal. The next scenario introduces a more efficient deterministic reduction built on warp primitives.

#### Precision Comparison and Analysis

The ULP error of each round's result against the Host-side sequential golden is as follows:

| Observation | Measured value |
| --- | --- |
| All 10 rounds' result bit patterns | 0x503ebc20 (12800000000.0), without exception |
| ULP vs golden | +0 (constant) |

Data analysis:

- **Constant means deterministic**: all 10 rounds matched bit for bit, showing that the fixed halving order is independent of scheduling for this input.

- **ULP error vs golden**: the zero error against the Host golden is coincidental because the two computations use different addition orders. Since the Host golden uses sequential addition, the halving tree may have a constant non-zero error for some inputs, which is normal.

### Scenario 3: Deterministic Reduction with the 3-layer Reduction Tree

The overall flow first: the 8192 inputs are reduced by 4 blocks (2048 threads each) to their own block sums s0-s3. L1 performs the butterfly reduction within each warp, L2 performs the second-level butterfly within each block, and L3 is the last-completed-block fixed-order merge producing the final result, as shown below.

<img src="./figures/deterministic_case2_struct.png">

The reduction tree has three layers:

- **L1 intra-warp butterfly reduction**: each warp executes 5 rounds of `asc_shfl_down` with delta halving from 16; in each round, lane and lane+delta are added, and lane 0 finally holds the sum of the 32 values of the warp. Threads in a warp are inherently lockstep, so no extra synchronization is needed. The code is as follows:

```cpp
__aicore__ inline float warp_butterfly_reduce(float val)
{
    val += asc_shfl_down(val, 16, WARP_SIZE);
    val += asc_shfl_down(val, 8, WARP_SIZE);
    val += asc_shfl_down(val, 4, WARP_SIZE);
    val += asc_shfl_down(val, 2, WARP_SIZE);
    val += asc_shfl_down(val, 1, WARP_SIZE);
    return val;
}
```

- **L2 intra-block second-level butterfly**: a UB staging buffer and a GM staging buffer are allocated; with 64 warps per 2048-thread block, two warps first reduce two groups of 32 partial sums, then warp 0 combines the two group results before writing the block result to GM;
- **L3 fixed-order block-sum merge**: thread 0 of the last-completed block accumulates the block sums in order to obtain the final result.

**Figure 4**  Computation flow of the butterfly tree (L1 intra-warp butterfly reduction, illustrated with 32 example values)

<img src="./figures/deterministic_case2_butterfly.png">

#### Precision Comparison and Analysis

The ULP error of each round's result against the sequential golden is as follows:

| Observation | Measured value |
| --- | --- |
| All 10 rounds' result bit patterns | 0x503ebc20 (12800000000.0), without exception |
| ULP vs golden | +0 (constant) |
| Relative error | 0 (constant) |

Data analysis:

- **Constant error, no non-determinism**: the butterfly tree's ULP error was constantly +0 over the 10 rounds, showing that the pairing order is fixed in software and independent of run-time scheduling.

- **ULP error vs golden**: the butterfly tree's zero error against the Host golden is coincidental because the two computations use different addition orders.

### Deterministic Computation Summary

Cases 0-2 show that floating-point accumulation with atomic APIs may produce different results across runs. To guarantee deterministic computation, the user must implement a fixed computation order.

Deterministic algorithms differ in ease of use and performance. Cases 1 and 2 process the same amount of data, and their end-to-end durations are as follows:

| Case | Kernel | Task Duration(μs) |
| ---- | ---- | :----------------: |
| 1 | `kernel_halving_reduce` | 5.483 |
| 2 | `kernel_deterministic_reduce` | 3.482 |

The butterfly tree performs better than the halving tree. Both trees perform the same computation, and the gap mainly comes from the data exchange path and the number of synchronizations:

- **Data exchange path**: all 32 intra-warp elements in L1 and the 64 warp partial sums in L2 are exchanged through `asc_shfl_down` between registers in the butterfly tree. Only the 64 warp partial sums pass through UB once per block. In the halving tree, all 2048 elements reside in the dynamic UB staging buffer, and every active thread performs a "read UB → add → write back UB" round trip in each round.
- **Synchronization count**: to guarantee "the whole group writes first, then reads" in every round, the halving tree executes 11 `asc_syncthreads` per block, while the current 64-warp butterfly implementation requires 2 block-level synchronizations.

Both trees are bit-deterministic, so the choice is a trade-off: the halving tree does not rely on warp exchange primitives and is conceptually straightforward; the butterfly tree exchanges data through registers with fewer synchronizations and is recommended for latency-sensitive reduction scenarios.

## Build and Run

Perform the following steps in the root directory of this example to build and run it.

- Configure environment variables

  Configure the environment variables according to the [installation method](../../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN installation directory. If no installation directory is specified during installation, the default path is `/usr/local/Ascend`.

- Run the example

  Execute the following commands in the example directory.

  ```bash
  mkdir build && cd build;                                      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;          # Build the project
  ./deterministic_reduce 0                                      # Run Scenario 1: atomic non-deterministic accumulation
  ./deterministic_reduce 1                                      # Run Scenario 2: deterministic reduction (halving tree)
  ./deterministic_reduce 2                                      # Run Scenario 3: deterministic reduction (butterfly tree)
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` option.

  For example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # NPU simulation mode
  ```

  > **Note:** Before switching build modes, clean the cmake cache. Run `rm CMakeCache.txt` in the build directory and run cmake again.

  Build option description

  | Option | Value | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU run, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example supports only dav-3510 (Ascend 950PR&950DT products) |
  | Runtime argument | `0`, `1`, `2` | Scenario number: 0 for the atomic non-deterministic accumulation scenario, 1 for the deterministic reduction scenario (halving tree), 2 for the deterministic reduction scenario (butterfly tree) |

  - Scenario 1 output is as follows: the ULP errors are not fixed, which verifies the non-deterministic computation. If a run happens to show identical ULP errors, it is only an occasional scheduling that just masked the rounding differences.

    ```text
    Input: 8192 floats (fixed formula), 10 rounds, golden (sequential sum) = 12800000000.000000 (bits: 0x503ebc20)
    Case 0 (atomic accumulation, order decided by scheduling): 4 blocks x 2048 threads, 10 rounds, input = 8192 floats (fixed formula)

    ===== Case 0: atomic accumulation summary (10 rounds) =====
    Round  0: 12799997952.000000 (bits: 0x503ebc1e) ULP vs golden: -2
    Round  1: 12799993856.000000 (bits: 0x503ebc1a) ULP vs golden: -6
    Round  2: 12799993856.000000 (bits: 0x503ebc1a) ULP vs golden: -6
    Round  3: 12799994880.000000 (bits: 0x503ebc1b) ULP vs golden: -5
    Round  4: 12799993856.000000 (bits: 0x503ebc1a) ULP vs golden: -6
    Round  5: 12799995904.000000 (bits: 0x503ebc1c) ULP vs golden: -4
    Round  6: 12799995904.000000 (bits: 0x503ebc1c) ULP vs golden: -4
    Round  7: 12799994880.000000 (bits: 0x503ebc1b) ULP vs golden: -5
    Round  8: 12799995904.000000 (bits: 0x503ebc1c) ULP vs golden: -4
    Round  9: 12799995904.000000 (bits: 0x503ebc1c) ULP vs golden: -4
    Rounds bit-identical to round 0: 0/9
    Golden (sequential sum): 12800000000.000000 (bits: 0x503ebc20)
    Note: 9/9 rounds differ from round 0 due to non-deterministic atomic accumulation order (ULP errors vary across rounds).
    ```
  - Scenario 2 output is as follows: the ULP error is a constant value, and the determinism verification passed.

    ```text
    Input: 8192 floats (fixed formula), 10 rounds, golden (sequential sum) = 12800000000.000000 (bits: 0x503ebc20)
    Case 1 (deterministic reduction, fixed-order block halving): 4 blocks x 2048 threads, 10 rounds, input = 8192 floats (fixed formula)

    ===== Case 1: deterministic reduction (halving) summary (10 rounds) =====
    Round  0: 12800000000.000000 (bits: 0x503ebc20) ULP vs golden: +0
    ...
    Round  9: 12800000000.000000 (bits: 0x503ebc20) ULP vs golden: +0
    Rounds bit-identical to round 0: 9/9
    Golden (sequential sum): 12800000000.000000 (bits: 0x503ebc20)
    [Success] Deterministic path: all 10 rounds bit-identical; ULP error vs sequential golden is constant (+0), a fixed algorithm-order deviation without non-determinism.
    ```

  - Scenario 3 output is as follows: the ULP error is a constant value, and the determinism verification passed.

    ```text
    Input: 8192 floats (fixed formula), 10 rounds, golden (sequential sum) = 12800000000.000000 (bits: 0x503ebc20)
    Case 2 (deterministic reduction, fixed-order 3-layer butterfly tree): 4 blocks x 2048 threads, 10 rounds, input = 8192 floats (fixed formula)

    ===== Case 2: deterministic reduction (butterfly) summary (10 rounds) =====
    Round  0: 12800000000.000000 (bits: 0x503ebc20) ULP vs golden: +0
    ...
    Round  9: 12800000000.000000 (bits: 0x503ebc20) ULP vs golden: +0
    Rounds bit-identical to round 0: 9/9
    Golden (sequential sum): 12800000000.000000 (bits: 0x503ebc20)
    [Success] Deterministic path: all 10 rounds bit-identical; ULP error vs sequential golden is constant (+0), a fixed algorithm-order deviation without non-determinism.
    ```

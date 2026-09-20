# In-Place Add Instruction Optimization Sample

## Overview

This sample uses two in-place accumulation operations — three-operand multiply-add (`a[index] += b[index] * c[index]`) and two-operand add (`a[index] += b[index]`) — as vehicles to demonstrate how to optimize performance with atomic add.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | \>= CANN 9.2.0 |

## Directory Structure

```text
├── inplace_add_atomic
│   ├── figures                  // Figures for the sample documentation
│   ├── CMakeLists.txt           // Sample build script
│   ├── inplace_add_atomic.asc   // Ascend C SIMT kernel implementation & Host invocation
│   ├── README.md                // Sample documentation
│   └── README_en.md             // English sample documentation
```

## Sample Description

This sample performs element-wise accumulation on `int32_t` arrays, consisting of two comparison groups:

- Three-operand multiply-add (Case 0, Scenarios 0/1): performs vector multiply-add on three arrays, accumulating the element-wise products of `b` and `c` onto `a`.

  ```text
  a[index] += b[index] * c[index],  index = 0, 1, ..., element_count - 1
  ```

- Two-operand add (Case 1, Scenarios 2/3): performs vector add on two arrays, accumulating the elements of `b` onto `a`.

  ```text
  a[index] += b[index],  index = 0, 1, ..., element_count - 1
  ```

- Sample Specifications:

  <table>
  <tr><td align="center">Sample Type (OpType)</td><td colspan="4" align="center">Atomic Optimization</td></tr>
  <tr><td rowspan="4" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[4194304]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[4194304]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td align="center">c</td><td align="center">[4194304]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Sample Output</td><td align="center">a</td><td align="center">[4194304]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Name</td><td colspan="4" align="center">vector_muladd_plain / vector_muladd_atomic / vector_add_plain / vector_add_atomic</td></tr>
  </table>

## Sample Implementation

This section analyzes the two controlled groups (Case 0~1) one by one, covering the impact of the accumulation method on performance under two computation forms: three-operand multiply-add and two-operand add. Each group changes only the accumulation method while keeping everything else identical, and provides the corresponding `msopprof` measured data and root-cause analysis of the performance behavior.

### Performance Metric

| Metric | Description |
| --- | --- |
| Task Duration (μs) | Total Task time, including scheduling to the accelerator, execution time on the accelerator, and response completion time |

---

### Case 0: Three-operand multiply-add

**Goal**: Compare the performance difference between `a[index] += b[index] * c[index]` and `asc_atomic_add(&a[index], b[index] * c[index])`.

**Scenario configuration**:

| Scenario | Kernel | Accumulation Method |
|:---:|---|---|
| 0 | vector_muladd_plain | a[index] += b[index] * c[index] |
| 1 | vector_muladd_atomic | asc_atomic_add(&a[index], b[index] * c[index]) |

**Core implementation**: Launches 64 thread blocks (the AIV core count queried at runtime), each with 2048 threads. Each thread traverses the array with a stride equal to the total number of threads. The two scenarios differ only in the accumulation statement of the loop body: Scenario 0 uses a plain in-place add, while Scenario 1 uses `asc_atomic_add`. The kernel implementations are as follows.

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_muladd_plain(
    int32_t* a, const int32_t* b, const int32_t* c, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        a[index] += b[index] * c[index];
    }
}

__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_muladd_atomic(
    int32_t* a, const int32_t* b, const int32_t* c, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        asc_atomic_add(&a[index], b[index] * c[index]);
    }
}
```

**Performance data**:

| Scenario | Accumulation Method | Task Duration (μs) | Duration Relative to Baseline |
|:---:|:---:|:---:|:---:|
| 0 | a[index] += b[index] * c[index] | 51.62 | 1× |
| 1 | asc_atomic_add | 47.51 | **0.92×** |

**Analysis**:

The performance data shows one phenomenon: switching to atomic add reduces the Task Duration by 8.0%. The difference comes solely from the data path of the old value of the accumulation target `a[index]`.

The following figure compares the data flow of the two accumulation methods in this sample: the left side is Scenario 0 (plain in-place add), the right side is Scenario 1 (atomic add), and GM on both sides holds the three arrays `a`, `b`, and `c`.

![Data flow comparison of the two accumulation methods](figures/inplace_add_atomic_dataflow.png)

- **In-place add (Scenario 0)**: `a[index]`, `b[index]`, and `c[index]` are all read into registers through L2 Cache, and after the multiply-add completes, the new value of `a[index]` is written back from the registers to L2 Cache.
- **Atomic add (Scenario 1)**: only `b[index]` and `c[index]` are read into registers to compute the product, which is issued as the operand of the atomic add; reading the old value, the addition, and the write-back are completed as one indivisible unit at the L2 Cache side, so `a[index]` never enters the registers and the round trip above does not happen.

In both scenarios, the reads of `b` and `c` and the multiplication are completely identical; the only variable is the elimination of the data round trip of the old value of `a[index]` in and out of the registers. The 8.0% gap comes precisely from this.

**Conclusion**: For element-wise accumulation, prefer completing the accumulation with atomic add in L2.

---

### Case 1: Two-operand add

**Goal**: Compare the performance difference between `a[index] += b[index]` and `asc_atomic_add(&a[index], b[index])`.

**Scenario configuration**:

| Scenario | Kernel | Accumulation Method |
|:---:|---|---|
| 2 | vector_add_plain | a[index] += b[index] |
| 3 | vector_add_atomic | asc_atomic_add(&a[index], b[index]) |

**Core implementation**: Launches 64 thread blocks (the AIV core count queried at runtime), each with 2048 threads. Each thread traverses the array with a stride equal to the total number of threads. The two scenarios differ only in the accumulation statement of the loop body: Scenario 2 uses a plain in-place add, while Scenario 3 uses `asc_atomic_add`. The kernel implementations are as follows.

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_add_plain(
    int32_t* a, const int32_t* b, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        a[index] += b[index];
    }
}

__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_add_atomic(
    int32_t* a, const int32_t* b, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        asc_atomic_add(&a[index], b[index]);
    }
}
```

**Performance data**:

| Scenario | Accumulation Method | Task Duration (μs) | Duration Relative to Baseline |
|:---:|:---:|:---:|:---:|
| 2 | a[index] += b[index] | 39.18 | 1× |
| 3 | asc_atomic_add | 36.52 | **0.93×** |

**Analysis**:

The performance data shows one phenomenon: switching to atomic add reduces the Task Duration by 6.8%. The principle is the same as in Case 0.

**Conclusion**: Two-operand addition benefits from the same optimization.

---

## Performance Comparison Summary

**Full-scenario Task Duration summary**:

| Scenario | Case | Computation Form | Accumulation Method | Task Duration (μs) | Duration Relative to Baseline |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 0 | Case 0 | Three-operand multiply-add | a[index] += b[index] * c[index] | 51.62 | 1× |
| 1 | Case 0 | Three-operand multiply-add | asc_atomic_add | 47.51 | **0.92×** |
| 2 | Case 1 | Two-operand add | a[index] += b[index] | 39.18 | 1× |
| 3 | Case 1 | Two-operand add | asc_atomic_add | 36.52 | **0.93×** |

The conclusion of both controlled groups is consistent: the scenarios using atomic add are both faster than their baselines, with latency reduced by 8.0% in the multiply-add group and 6.8% in the add group.

## Tuning Recommendations

1. **Prefer atomic add for element-wise in-place accumulation**: When the old value of the target address is used only for accumulation and does not participate in other computations, switching to atomic add completes the read, addition, and write-back of the old value in L2, eliminating the data round trip of the old value in and out of the registers.
2. **Note the applicability boundary**: This optimization applies only to element-wise in-place updates; before use, confirm that the target addresses of each thread's atomic operations do not overlap. Its boundary differs from reduction-style accumulation (multiple threads accumulating into the same address, e.g. the histogram counting in the [atomic_histogram](../atomic_histogram/README_en.md) sample): in reduction accumulation, atomic operations on the same address can only execute serially, so under address contention, using atomic add directly degrades performance instead; use the block-local accumulation and merge strategy shown in that sample.

## Build and Run

Run the following steps in the root directory of this sample to build and execute the sample.

- Configure Environment Variables

  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Sample

  Scenarios are selected at compile time via `SCENARIO_NUM`; each build contains exactly one scenario. Run the following commands in this sample directory.

  ```bash
  mkdir -p build && cd build                                      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=0 ..    # Configure the project (Scenario 0: multiply-add baseline)
  make -j                                                         # Build the sample
  ./inplace_add_atomic                                            # Run Scenario 0
  ```

  To switch scenarios, reconfigure `SCENARIO_NUM`, rebuild, and run:

  ```bash
  cmake -DSCENARIO_NUM=1 .. && make -j && ./inplace_add_atomic    # Scenario 1: multiply-add optimization
  cmake -DSCENARIO_NUM=2 .. && make -j && ./inplace_add_atomic    # Scenario 2: add baseline
  cmake -DSCENARIO_NUM=3 .. && make -j && ./inplace_add_atomic    # Scenario 3: add optimization
  ```

  When using NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # NPU simulation mode
  ```

  > **Note:** Clear the cmake cache before switching build modes. Execute `rm CMakeCache.txt` in the build directory, then run cmake again.

  Build Options Description:

  | Option | Values | Description |
  |------|--------|------|
  | CMAKE_ASC_RUN_MODE | npu/sim | Run mode: NPU execution, NPU simulation, default npu |
  | CMAKE_ASC_ARCHITECTURES | dav-3510 | NPU architecture: this sample only supports dav-3510 (Ascend 950PR/Ascend 950DT) |
  | SCENARIO_NUM | 0/1/2/3 | Scenario number: 0/1 are the baseline/optimized scenarios of Case 0 (three-operand multiply-add), 2/3 are the baseline/optimized scenarios of Case 1 (two-operand add) |
  | SKIP_VALIDATION | ON/OFF | Whether to skip result validation, default OFF. Recommended to set to ON when collecting performance with msopprof |

  The following output indicates that the accuracy verification is successful.

  ```text
  Scenario 0 (muladd plain update): 4194304 elements, 64 blocks, 2048 threads per block
  [Success] Case accuracy verification passed.
  ```

## Performance Debugging

### Introduction to the msOpProf Tool

`msOpProf` is a single-operator performance analysis tool. It offers two usage methods: `msopprof` and `msopprof simulator`. The tool helps users identify anomalies in operator memory, operator code, and operator instructions, enabling comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and different file types (executables or operator binary `.o` files).

Use the `msOpProf` tool to obtain detailed performance data. Each scenario must be built with that scenario before collection; take Scenario 1 as an example:

```bash
cmake -DSCENARIO_NUM=1 -DSKIP_VALIDATION=ON ..
make -j
msopprof ./inplace_add_atomic
```

> **Regarding Validation failed during performance collection:** In this sample, the accumulation target `a` is initialized on the host side at allocation, and the kernel only accumulates on it. The `msopprof` warmup+replay re-executes the kernel on the same GM memory, causing `a` to be accumulated multiple times; therefore, strict validation mode reports `Validation failed`. This is an inherent conflict between the replay mechanism and the validation logic. When collecting performance, it is recommended to rebuild with `-DSKIP_VALIDATION=ON` before running `msopprof` to skip validation.

After the command completes, a folder named "OPPROF_{timestamp}_XXX" is generated in the default directory. The performance data folder structure is as follows:

```text
├──dump                       # Raw performance data, users do not need to focus on this
├──ArithmeticUtilization.csv  # cube/vector instruction cycle ratio
├──L2Cache.csv                # L2 Cache hit rate
├──Memory.csv                 # UB, L1, and main memory read/write bandwidth rates
├──MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth rates
├──MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth rates
├──OpBasicInfo.csv            # Operator basic information
├──PipeUtilization.csv        # Compute unit and transfer unit time consumption and ratio
├──ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio across all instructions
└──visualize_data.bin         # MindStudio Insight presentation file
```

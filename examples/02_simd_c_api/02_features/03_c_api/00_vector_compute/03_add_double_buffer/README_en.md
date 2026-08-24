# Fused Vector Compute Double-Buffer Performance Sample

## Overview

This sample implements half fused vector computation with the Ascend C C API and compares single buffering with Ping/Pong double buffering. A build option changes the amount of register-only computation without increasing GM traffic, allowing double-buffer performance to be evaluated at different Vector compute intensities.

## Supported Products and CANN Versions

**Table1**  Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```text
03_add_double_buffer
├── figures
│   └── double_buffer.png    # Ping/Pong Buffer pipeline diagram
├── scripts
│   ├── gen_data.py          # Generates input and golden data
│   └── verify_result.py     # Verifies the output
├── CMakeLists.txt           # Build configuration
├── add_double_buffer.asc    # Kernel and host implementation
├── data_utils.h             # Binary file utilities
├── README.md
└── README_en.md
```

## Sample Description

The sample performs an element-wise fused computation on two half matrices with shape `[8192, 8192]`:

$$
z = x^{R+1} + y \sum_{i=0}^{R} x^i, \quad R = \text{FUSED\_ITERATIONS}
$$

Multiplication, powers, and summation in this expression are all element-wise. Intermediate values remain in Vector registers, so every tile still loads x and y once and stores z once.

`FUSED_ITERATIONS` accepts any positive integer and defaults to `8`. Each VF iteration executes `1 + 2 * FUSED_ITERATIONS` Vector arithmetic instructions. `FUSED_ITERATIONS=0` only represents the historical pure-Add configuration and is not a current configurable value.

The input, output, and golden data used for accuracy verification are listed below. All data is stored contiguously in ND format in GM.

**Table2**  Input and Output Data Specifications

| Data | Input/Output | Data file | Data type | Shape | Description |
| --- | --- | --- | --- | --- | --- |
| x | Input | `input/input_x.bin` | half | `[8192, 8192]` | Randomly generated in the range `[-0.1, 0.1)`. |
| y | Input | `input/input_y.bin` | half | `[8192, 8192]` | Randomly generated in the range `[-0.1, 0.1)`. |
| z | Output | `output/output.bin` | half | `[8192, 8192]` | Result computed by the Kernel. |
| golden | Golden data | `output/golden.bin` | half | `[8192, 8192]` | Generated in the same recurrence order as the Kernel, with conversion to half after each multiply and add. |

Both scenarios use the same core partitioning and tile parameters. The matrix is split along the M axis across 64 cores, with each core processing 128 contiguous rows. Table3 shows the calculations.

**Table3**  Multicore Partitioning and Tile Parameters

| Parameter | Value | Description |
| --- | --- | --- |
| Total matrix elements | 67108864 | `8192 * 8192` |
| Number of cores | 64 | Each core processes a contiguous range along the M axis. |
| Rows per core | 128 | `8192 / 64` |
| Elements per core | 1048576 | `128 * 8192` |
| Reserved length of each UB buffer | 21840 half elements | Corresponds to `MAX_DATA_COPY_LENGTH` and defines the UB address range of each buffer. |
| Valid elements in a full tile | 21760 | Corresponds to `DATA_COPY_LENGTH`. |
| Valid elements in the tail tile | 4096 | `1048576 % 21760` |
| Iterations per core | 49 | `1048576 / 21760 = 48` with 4096 elements remaining, giving 48 full tiles and one tail tile. |
| VF vector length | 256 bytes | Processes 128 half elements at a time; a full tile uses 170 VF iterations and the tail tile uses 32. |
| Fused recurrence iterations | Positive integer | Corresponds to `FUSED_ITERATIONS`; the default is `8`. |
| Arithmetic instructions per VF iteration | `1 + 2 * FUSED_ITERATIONS` | One initial Add; each recurrence step adds one Mul and one Add. |

The `SCENARIO_NUM` build option selects single buffering or Ping/Pong double buffering, and `FUSED_ITERATIONS` selects the recurrence count. Buffer scenarios must use the same `FUSED_ITERATIONS` value when their performance is compared.

### Scenario 0: Ping/Pong Buffer Optimization Disabled

`add_baseline` allocates one contiguous x/y/z buffer group. Each buffer reserves 21840 half elements, and `current_length` specifies the number of valid elements accessed in the current iteration.

<a id="figure-1"></a>
<p align="center">
  <img src="figures/sequential_pipeline.png" width="50%">
</p>
<p align="center">Figure 1. Ping/Pong Buffer Pipeline</p>

**Table4**  UB Layout with Ping/Pong Buffer Optimization Disabled

| UB buffer | Start offset (half elements) | Valid elements in a full tile | Purpose |
| --- | --- | --- | --- |
| `x_local` | 0 | 21760 | Stores x loaded from GM for the current iteration. |
| `y_local` | 21840 | 21760 | Stores y loaded from GM for the current iteration. |
| `z_local` | 43680 | 21760 | Stores the fused-compute result for the current iteration. |

Each iteration processes one tile in the following order:

1. MTE2 calls `asc_copy_gm2ub_align` twice to load the current x and y tiles from GM to UB.
2. After MTE2 releases Mutex 0, Vector acquires it and calls `fused_compute_vf`. Each VF iteration executes `1 + 2 * FUSED_ITERATIONS` arithmetic instructions.
3. Vector releases Mutex 0, after which MTE3 acquires it and calls `asc_copy_ub2gm_align` to store z in GM.
4. MTE3 releases Mutex 0 before MTE2 can overwrite the same UB addresses in the next iteration. After the loop, `asc_sync_pipe(PIPE_ALL)` waits for all operations to finish.

All three stages share one Mutex and run in the order MTE2, Vector, and MTE3. The next iteration cannot reuse UB until the current store finishes, so iterations do not overlap. This implementation serves as the performance baseline.

### Scenario 1: Ping/Pong Buffer Optimization Enabled

`add_double_buffer` allocates Ping and Pong x/y/z buffer groups in UB. Even iterations use Ping and odd iterations use Pong. Each group has two Mutexes: an input Mutex protects the loading and reading of x/y, and an output Mutex protects the writing and storing of z.

<a id="figure-2"></a>
<p align="center">
  <img src="figures/overlapped_pipeline.png" width="50%">
</p>
<p align="center">Figure 2. Ping/Pong Buffer Pipeline</p>

**Table5**  Buffer Layout with Ping/Pong Buffer Optimization Enabled

| Iteration | Buffer group | Input Mutex ID | Output Mutex ID | x start offset | y start offset | z start offset |
| --- | --- | --- | --- | --- | --- | --- |
| Even | Ping | 0 | 2 | 0 | 21840 | 43680 |
| Odd | Pong | 1 | 3 | 65520 | 87360 | 109200 |

Offsets are measured in half elements, and each buffer reserves 21840 elements. Double buffering does not change the amount of data moved or computed per iteration; `asc_lock` and `asc_unlock` only control when each buffer group can be reused.

**Table6**  Synchronization Dependencies with Ping/Pong Buffer Optimization Enabled

| Pipeline stage | Mutex ID | Synchronization operation | Enforced dependency |
| --- | --- | --- | --- |
| MTE2 load | Input Mutex: 0 for Ping, 1 for Pong | Acquire the input Mutex, load x and y, and then release it. | Before rewriting x and y in a group, wait for the preceding Vector stage to finish reading them. |
| Vector computation | Input and output Mutexes for the current group | Acquire both Mutexes, call `fused_compute_vf`, and then release both. | Wait for the current x/y load and for the preceding MTE3 stage to finish reading z in the same group. |
| MTE3 store | Output Mutex: 2 for Ping, 3 for Pong | Acquire the output Mutex, store z, and then release it. | Wait for the current Vector stage to finish writing z. |

`loop_idx & 1` selects Ping or Pong. Input Mutexes are 0/1, and the corresponding output Mutexes are 2/3. Once Vector has finished reading x/y, MTE2 can load the next tile into the same group's input buffers. MTE3 independently waits on the output Mutex for z to be ready. Because MTE2 does not access z and MTE3 does not access x/y, these transfers can run concurrently. After all 49 iterations have been issued, one `asc_sync_pipe(PIPE_ALL)` completes the final synchronization. See [asc_lock](../../../../../../docs/zh/api/SIMD-API/c_api/sync/asc_lock.md) and [asc_unlock](../../../../../../docs/zh/api/SIMD-API/c_api/sync/asc_unlock.md) for API details.

For example, iteration 0 uses Ping with Mutexes 0/2, iteration 1 uses Pong with 1/3, and iteration 2 returns to Ping with 0/2. Once iteration 0 Vector has finished reading x/y, iteration 2 MTE2 can overwrite Ping x/y while iteration 0 MTE3 is still reading Ping z. Iteration 2 Vector must still wait for that MTE3 store to finish before writing Ping z.

Ping/Pong Buffer uses two independent x/y/z buffer groups to process tiles alternately. This allows MTE2 transfer, Vector computation, and MTE3 store to overlap across iterations, at the cost of one additional UB buffer group.

<a id="figure-3"></a>
<p align="center">
  <img src="figures/capi_double_buffer.png" width="50%">
</p>
<p align="center">Figure 3. Ping/Pong Buffer Pipeline</p>

As shown in [Figure 3](#figure-3), MTE2, Vector, and MTE3 overlap while processing adjacent tiles.

## Performance Comparison

For a fixed `FUSED_ITERATIONS` value, the two scenarios change only the buffer count and synchronization. Keep the hardware, CANN version, input data, system load, compute level, and tiling parameters unchanged, and verify accuracy before collecting performance data.

### Test Conditions

**Table7**  Performance Test Configuration

| Configuration | Value |
| --- | --- |
| Chip version | Ascend 950PR |
| CANN version | CANN 9.2.0 |
| NPU architecture | `dav-3510` |
| Input/output data type | half |
| Input/output shape | `[8192, 8192]` |
| AI Vector Cores | 64 |
| Tile length | 21760 half elements; the tail tile contains 4096 half elements |
| Fused recurrence iterations | Positive integer; test both buffer scenarios with the same value |
| Number of measurements | Warm up each scenario once, collect at least five valid runs, and report the median Task Duration |

Keep the device frequency and power mode unchanged during testing, and ensure that no other task occupies the NPU. Use the same input data, core count, tiling parameters, and build options for both scenarios.

### Performance Metrics

Use msOpProf to collect performance data and inspect the following fields in `PipeUtilization.csv`.

**Table8**  Performance Metric Descriptions

| Metric | Description |
| --- | --- |
| `Task Duration(μs)` | Total Task execution time. Use this metric for the end-to-end comparison. |
| `aiv_time(μs)` | Task execution time on AI Vector Core. |
| `aiv_vec_time(μs)` / `aiv_vec_ratio` | Vector instruction duration and ratio. |
| `aiv_scalar_time(μs)` / `aiv_scalar_ratio` | Scalar instruction duration and ratio. |
| `aiv_mte2_time(μs)` / `aiv_mte2_ratio` | Duration and ratio of GM-to-UB transfers on the MTE2 pipeline. |
| `aiv_mte3_time(μs)` / `aiv_mte3_ratio` | Duration and ratio of UB-to-GM transfers on the MTE3 pipeline. |

Ping/Pong double buffering overlaps the pipelines, so their durations or ratios cannot be added directly. Use `Task Duration` for the end-to-end comparison and the other metrics to identify where time is spent.

### Ascend 950PR Performance Comparison

The original pure-Add configuration used `FUSED_ITERATIONS=0`. Its compute workload was small and the double-buffer benefit was limited, so this section omits those historical measurements.

**Table9**  Performance Data with `FUSED_ITERATIONS=12`

| Scenario | Configuration | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Scenario 0 | Ping/Pong Buffer optimization disabled | 285.401 | 284.24 | 120.175 | 0.423 | 2.12 | 0.007 | 143.849 | 0.506 | 16.041 | 0.056 |
| Scenario 1 | Ping/Pong Buffer optimization enabled | 269.418 | 268.35 | 118.935 | 0.443 | 1.959 | 0.007 | 262.743 | 0.979 | 142.026 | 0.529 |

**Table10**  Performance Data with `FUSED_ITERATIONS=14`

| Scenario | Configuration | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Scenario 0 | Ping/Pong Buffer optimization disabled | 296.725 | 295.62 | 142.654 | 0.483 | 1.842 | 0.006 | 132.634 | 0.449 | 16.228 | 0.055 |
| Scenario 1 | Ping/Pong Buffer optimization enabled | 271.788 | 270.69 | 142.011 | 0.525 | 2.216 | 0.008 | 263.457 | 0.973 | 138.192 | 0.511 |

**Table11**  Performance Data with `FUSED_ITERATIONS=24`

| Scenario | Configuration | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Scenario 0 | Ping/Pong Buffer optimization disabled | 433.267 | 432.46 | 306.585 | 0.709 | 1.72 | 0.004 | 105.532 | 0.244 | 16.266 | 0.038 |
| Scenario 1 | Ping/Pong Buffer optimization enabled | 306.767 | 305.87 | 298.238 | 0.975 | 2.401 | 0.008 | 150.555 | 0.492 | 15.939 | 0.052 |

#### Performance Analysis

With `FUSED_ITERATIONS=14`, the Scenario 0 stage-balance error between VEC and MTE2 is:

$$
\left|T_{\text{vec}} - T_{\text{mte2}}\right| = \left|142.654 - 132.634\right| = 10.020\ \mu s
$$

This is the smallest error among the measured levels, indicating balanced compute and load durations. The Ping/Pong Buffer end-to-end reduction is:

$$
\frac{T_{\text{baseline}} - T_{\text{ping-pong}}}{T_{\text{baseline}}} \times 100\%
= \frac{296.725 - 271.788}{296.725} \times 100\%
= 8.40\%
$$

In Scenario 1, `aiv_mte2_ratio` is 97.3%, so MTE2 remains the main pipeline bottleneck. Pipeline metrics include overlap and wait time, and must not be added together or used to compare individual transfer speed; use `Task Duration` for the performance conclusion.

#### Fused-Compute Level Measurements

Table12 summarizes three representative measured levels. The stage-balance error is the absolute difference between `aiv_vec_time` and `aiv_mte2_time` in Scenario 0.

**Table12**  Fused-Compute Performance Records

| `FUSED_ITERATIONS` | Arithmetic instructions per VF iteration | Scenario 0 stage-balance error(μs) | Scenario 0 Task Duration(μs) | Scenario 1 Task Duration(μs) | Same-level duration reduction |
| --- | --- | --- | --- | --- | --- |
| 12 | 25 | 23.674 | 285.401 | 269.418 | 5.60% |
| 14 | 29 | 10.020 | 296.725 | 271.788 | 8.40% |
| 24 | 49 | 201.053 | 433.267 | 306.767 | 29.20% |

At `12`, transfer time is slightly higher than compute time; `14` is the closest measured balance point; and `24` represents a clearly compute-dominant workload. `FUSED_ITERATIONS=14` therefore remains the primary analysis configuration. The same-level reduction is higher at `24`, but that configuration also performs more fused computation. Across different counts, use the results only to observe workload changes; do not compare `Task Duration` directly.

Regenerate `golden.bin` for every fused-compute level, and verify both Scenarios 0 and 1. Different `FUSED_ITERATIONS` values represent different computations, so their Task Durations must not be used directly to calculate double-buffer speedup.

#### Conclusion

- **Measured benefit**: With `FUSED_ITERATIONS=14`, `Task Duration` decreases from 296.725μs to 271.788μs, saving 24.937μs or 8.40%.
- **Source of benefit**: The two scenarios use the same fused computation and data-transfer volume, and their `aiv_vec_time` values are nearly unchanged. The benefit comes from the buffer layout and pipeline overlap, not from less computation.
- **Pipeline advantage**: Double buffering adds one UB buffer group, allowing MTE2 to load the next tile, Vector to compute the current tile, and MTE3 to store the preceding tile in an interleaved schedule. MTE2 loading and MTE3 storing can also run concurrently.

## Build and Run

Configure the CANN environment:

```bash
source ${install_path}/cann/set_env.sh
```

Build and run one scenario from this sample directory:

```bash
SCENARIO_NUM=0
FUSED_ITERATIONS=8
RUN_MODE=npu
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 \
    -DCMAKE_ASC_RUN_MODE=${RUN_MODE} \
    -DSCENARIO_NUM=${SCENARIO_NUM} \
    -DFUSED_ITERATIONS=${FUSED_ITERATIONS} ..
make -j
python3 ../scripts/gen_data.py --fused-iterations ${FUSED_ITERATIONS}
./demo
python3 ../scripts/verify_result.py output/output.bin output/golden.bin
```

The default is NPU execution mode. For NPU simulation, set `RUN_MODE` to `sim`; the remaining build, data generation, execution, and verification steps are unchanged:

```bash
RUN_MODE=sim
```

NPU simulation is intended for Kernel functional and accuracy verification. Simulation timing must not be used to evaluate double-buffer performance; use on-device data collected in NPU execution mode for performance comparisons.

The CMake value and the `gen_data.py` argument must match when `FUSED_ITERATIONS` changes. Set `SCENARIO_NUM=1` to test Ping/Pong double buffering with the same fused computation. Clear the CMake cache before switching the run mode or other build options.

**Table13**  Build Options

| Build option | Values | Description |
| --- | --- | --- |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | Specifies the NPU architecture. |
| `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Selects NPU execution or NPU simulation mode. |
| `SCENARIO_NUM` | `0`, `1` (default) | Selects whether to enable Ping/Pong Buffer optimization. |
| `FUSED_ITERATIONS` | Positive integer; default `8` | Number of $t_{i+1}=t_i \times x+y$ recurrence steps. |

A successful verification prints:

```text
test pass!
```

## Functional Debugging

### printf

This interface provides formatted output for CPU-side and NPU-side debugging. In NPU execution and NPU simulation modes, this sample can call the [printf](../../../../../../docs/zh/api/Utils-API/tuning_interface/printf.md) interface directly wherever logging is needed in the Kernel. Include `utils/debug/asc_printf.h` before using the interface.

For example:

```cpp
printf("add blockIdx=%ld\n", static_cast<int64_t>(block_idx));
```

> **Note:** The `printf` interface increases Kernel execution time and affects performance measurements. Use it only during functional debugging. Remove or disable printing before performance collection, or set `ASCENDC_DUMP=0` to disable printing as needed.

### msOpProf Tool Introduction

msOpProf is a single-operator performance analysis tool. It provides `msopprof` and `msopprof simulator` modes to help locate issues in operator memory access, code, and instructions. It supports performance-data collection and automatic parsing in NPU and simulation modes, using either executable files or operator binary `.o` files.

#### NPU Performance Collection

NPU collection measures the execution time of an operator on the Ascend AI Processor and is useful for quickly locating operator performance issues on a device.

Run msOpProf with the `demo` executable:

```bash
msopprof ./demo
```

**Performance Data**

The command creates an `OPPROF_{timestamp}_XXX` directory. Its performance files include:

```text
├── dump                       # Raw performance data; normally not needed for analysis
├── ArithmeticUtilization.csv  # Cycle ratios of Cube and Vector instructions
├── L2Cache.csv                # L2 Cache hit rate, which affects MTE2
├── Memory.csv                 # Read/write bandwidth of UB, L1, and main memory
├── MemoryL0.csv               # Read/write bandwidth of L0A, L0B, and L0C
├── MemoryUB.csv               # Read/write bandwidth between Vector/Scalar and UB
├── OpBasicInfo.csv            # Basic operator information
├── PipeUtilization.csv        # Duration and ratio of compute and data-transfer pipelines
├── ResourceConflictRatio.csv  # UB bank-group, bank-conflict, and resource-conflict ratios
└── visualize_data.bin         # Data for MindStudio Insight visualization
```

View the detailed performance results:

```bash
# View Task Duration and pipeline metrics
cat ./OPPROF_*/PipeUtilization.csv
```

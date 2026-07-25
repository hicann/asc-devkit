# SIMD and SIMT Hybrid Programming Fast Integer Division Operator Example

## Overview

This example uses fixed-divisor integer division computation to demonstrate integer division optimization methods in SIMD and SIMT hybrid programming scenarios. The example shows the performance differences between directly using standard division within SIMT threads and using multiplication and shift to replace standard division after precomputing parameters.

**Optimization Path**:

| Case | SCENARIO_NUM | Implementation | Description |
|:---|:---:|:---|:---|
| Case 0 | 0 | Standard division | SIMT threads directly use `/` for integer division, serving as the baseline version before fixed-divisor division optimization. |
| Case 1 | 1 | Fast division | Precompute `magic` and `shift` for the fixed divisor. SIMT threads use multiplication and shift to replace standard division. |

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Versions

- \>= CANN 9.1.0

## Directory Structure

```
├── simd_simt_integer_fast_div
│   ├── CMakeLists.txt          // Build project file
│   ├── integer_div.asc         // Integer division example implementation
│   └── README.md
```

## Example Description

**Example Function**:

This example implements fixed-divisor uint32 integer division computation and compares the performance differences between two SIMT thread computation methods: standard division and fast division. The computation formula is as follows:

```cpp
output[i] = input[i] / divisor
```

- input is the input data with shape [N], data type uint32, and value range [0, INT32_MAX]
- divisor is the fixed divisor with data type uint32
- output is the output data with shape [N] and data type uint32

**Example Specifications**:

<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">integer_div</td></tr>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">input</td><td align="center">[8388608]</td><td align="center">uint32</td><td align="center">ND</td></tr>
<tr><td align="center">divisor</td><td align="center">scalar</td><td align="center">uint32</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[8388608]</td><td align="center">uint32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center"><code>integer_div_kernel</code></td></tr>
</table>

## Example Implementation

Both cases use the same data transfer flow: first transfer the data assigned to the current thread block from GM to UB, the SIMT VF completes the division computation on UB and writes results to UB, and finally writes results from UB back to GM. This avoids data transfer differences affecting performance comparison, so the performance differences primarily come from standard division versus fast division themselves.

The shared data transfer and SIMT invocation flow for both cases is as follows:

```cpp
asc_copy_gm2ub_align(input_buf, input + block_offset, 1, blk_length, 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);

if ASC_IS_AIV {
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
}

if constexpr (scenario == 0) {
    asc_vf_call<simt_normal_div>(dim3(THREAD_COUNT), input_buf, output_buf, divisor, total_length);
} else {
    asc_vf_call<simt_fast_div>(dim3(THREAD_COUNT), input_buf, output_buf, magic, shift, total_length);
}

if ASC_IS_AIV {
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
}

asc_copy_ub2gm_align(output + block_offset, output_buf, 1, blk_length, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
```

### Performance Metric Description

| Field Name | Field Description |
|:---|:---|
| Task Duration(μs) | Total task latency, including scheduling time to the accelerator, execution time on the accelerator, and response completion time. |
| aiv_time(μs) | Task execution time on the AI Vector Core. |
| aiv_vec_time(μs) | Computation instruction latency. Thread computation in SIMT VF calls is also reflected in this metric. |
| aiv_vec_ratio | Ratio of computation instruction cycles to total cycles. |
| aiv_scalar_time(μs) | scalar-type instruction latency. |
| aiv_scalar_ratio | Ratio of scalar-type instruction cycles to total cycles. |
| aiv_mte2_time(μs) | mte2-type instruction latency, primarily corresponding to GM-to-UB transfers. |
| aiv_mte2_ratio | Ratio of mte2-type instruction cycles to total cycles. |
| aiv_mte3_time(μs) | mte3-type instruction latency, primarily corresponding to UB-to-GM transfers. |
| aiv_mte3_ratio | Ratio of mte3-type instruction cycles to total cycles. |

Except for Task Duration, all other metrics are averages across all Thread Blocks.

### Case 0: Standard Division Version

**Example Objective**: Implement basic integer division functionality as a latency comparison baseline for the fast division version.

**Core Implementation**:

- Each SIMT thread reads 1 input element from UB and directly uses `/` for integer division.

**Key Code**:

The `simt_normal_div()` function performs standard division on the input element `value` processed by the current thread:

```cpp
uint32_t result = value / divisor;
```

**Performance Data**:

| Case | Implementation | Data Volume | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Case 0 | Standard division | 8388608 | 117.229 | 1.277 | 0.497 | 0.390 | 0.128 | 0.099 | 0.423 | 0.331 | 0.222 | 0.174 |

**Performance Data Analysis**:

- Case 0's `Task Duration` is 117.229μs, serving as the comparison baseline for the fast division version.
- Case 0's `aiv_time` is 1.277μs, of which `aiv_vec_time` is 0.497μs, serving as the baseline for AI Vector Core execution time and computation instruction latency in the standard division version.

---

### Case 1: Fast Division Version

**Optimization Objective**: For integer division scenarios with fixed divisors, use multiplication and shift to replace standard division, reducing the division computation overhead within SIMT threads.

**Core Implementation**:

- On the SIMD side, compute the `magic` and `shift` required for fast division based on the fixed divisor. The computation results can be reused by SIMT threads.
- Each SIMT thread reads 1 input element from UB and uses `__umulhi` and right shift within the thread to complete the equivalent division computation.

#### Fast Division Conversion Principle

For uint32 integer division, the following transformation can convert division computation to multiplication and shift operations:

$$
\left\lfloor \frac{n}{d} \right\rfloor =
\left\lfloor \frac{n}{d} \times \frac{2^s}{2^s} \right\rfloor =
\left\lfloor \frac{2^s}{d} \times \frac{n}{2^s} \right\rfloor =
\left\lfloor {m} \times \frac{n}{2^s} \right\rfloor
$$

Where $n$ is the dividend, $d$ is the divisor, and $m = \frac{2^s}{d}$. Dividing $n$ by $2^s$ can be implemented by right-shifting $s$ bits. To avoid overflow in $m \times n$ multiplication, $m$ is decomposed as:

$$
m = (m - 2^{32}) + 2^{32}
$$

Let $magic = m - 2^{32}$. Substituting into the above formula yields:

$$
\left\lfloor \frac{n}{d} \right\rfloor =
\left\lfloor \frac{n \times {magic} + n \times 2^{32}}{2^s} \right\rfloor =
\left\lfloor \left(\frac{n \times magic}{2^{32}} + n\right) >> (s-32) \right\rfloor =
\left\lfloor \left(\frac{n \times magic}{2^{32}} + n\right) >> shift \right\rfloor
$$

Where `magic` and `shift` represent the multiplication magic number and shift bits required for fast division.
In this example, based on the above division conversion approach, the SIMD part uses the `clz()` and `bcnt1()` scalar computation interfaces in the Scalar computation unit to obtain the `magic` and `shift` required for fast division, for reuse by SIMT threads.

**Key Code**:

The `calc_magic_shift()` function computes `magic` and `shift` for fast division:

```cpp
int64_t pos = BIT_64_LEN - count_leading_zero(divisor);
int64_t cnt1 = get_bit_count1(divisor);
uint32_t shift = (cnt1 == 1) ? (pos - 1) : pos;
uint32_t magic = (1l << BIT_32_LEN) * ((1l << shift) - divisor) / divisor + 1;
```

The `simt_fast_div()` function performs fast division on the input element `value` processed by the current thread:

```cpp
uint32_t q = __umulhi(value, magic);
uint32_t result = (value + q) >> shift;
```

**Performance Data**:

| Case | Implementation | Data Volume | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Case 1 | Fast division | 8388608 | 103.889 | 1.068 | 0.270 | 0.254 | 0.142 | 0.132 | 0.425 | 0.398 | 0.221 | 0.208 |

**Performance Data Analysis**:

- Compared with the standard division version in Case 0, Case 1's `Task Duration` decreases from 117.229μs to 103.889μs, and `aiv_time` decreases from 1.277μs to 1.068μs, with overall execution latency reduced.
- `aiv_vec_time` decreases from 0.497μs to 0.270μs, indicating that after replacing standard division with multiplication and shift, computation instruction latency is significantly reduced.
- `aiv_scalar_time` increases from 0.128μs in Case 0 to 0.142μs, which is related to the additional scalar operations for computing `magic` and `shift` in the fast division scenario. However, the overall `Task Duration` still decreases, indicating that the benefits from fast division can cover this overhead.

---

## Performance Comparison Summary

### Ascend 950PR Performance Data

**Overall Optimization Effect**:

- Through the fast division optimization from Case 0 to Case 1, the example `Task Duration` decreases from 117.229μs to 103.889μs, a latency reduction of approximately 11.4%.
- Case 1 achieves approximately 1.13x performance improvement relative to Case 0, indicating that in fixed-divisor scenarios, using multiplication and shift to replace standard division can reduce end-to-end latency.

| Case | Implementation | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Case 0 | Standard division | 117.229 | 1.277 | 0.497 | 0.390 | 0.128 | 0.099 | 0.423 | 0.331 | 0.222 | 0.174 |
| Case 1 | Fast division | 103.889 | 1.068 | 0.270 | 0.254 | 0.142 | 0.132 | 0.425 | 0.398 | 0.221 | 0.208 |

### Optimization Key Points Summary

| Optimization Method | Core Principle | Example Demonstration |
|:---|:---|:---|
| Fixed divisor precomputation | When the divisor is fixed during kernel function execution, `magic` and `shift` can be computed in advance, and SIMT threads reuse these results for fast division. | Case 1 calls `calc_magic_shift()` in the kernel function, and SIMT threads no longer directly execute `/`. |
| Multiplication and shift replacing standard division | Standard integer division instructions have high overhead. Using multiplication and shift operations reduces per-element computation overhead. | Case 1 reduces `Task Duration` from 117.229μs to 103.889μs compared to Case 0. |

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Switch Cases

  Specify the Case to build via `-DSCENARIO_NUM=N` during cmake compilation. Case descriptions:

  - `0`: Standard division
  - `1`: Fast division

  Example:

  ```bash
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1
  ```

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=0                                                           # Select the execution scenario
  mkdir -p build && cd build;                                          # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # Build the project
  ./integer_div                                                        # Run the example
  ```

- Build option description

  | Option | Values | Description |
  |:---|:---|:---|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture, corresponding to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`, `1` | Case number: 0=standard division version, 1=fast division version |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```text
  [Success] Case accuracy is verification passed.
  ```

### Performance Analysis

### Introduction to the msOpProf Tool

`msOpProf` is a single-operator performance analysis tool. It offers two usage methods: `msopprof` and `msopprof simulator`. The tool helps users identify anomalies in operator memory, operator code, and operator instructions, enabling comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and different file types (executables or operator binary `.o` files).

- On-device performance collection

    On-device performance collection directly measures the execution time of an operator on an Ascend AI Processor. This method is suitable for quickly locating operator performance issues in an on-device environment.

    Use the `msOpProf` tool to obtain detailed performance data.

    ```bash
    msopprof ./integer_div   # Analyze performance
    ```

    - Performance data description  
      After the command completes, a folder named "OPPROF_{timestamp}_XXX" will be generated in the default directory. The performance data folder structure is as follows:

      ```bash
      ├──dump                       # Raw performance data; users do not need to inspect it
      ├──ArithmeticUtilization.csv  # Cube/Vector instruction cycle proportions
      ├──L2Cache.csv                # L2 Cache hit rate; affects MTE2. Plan data transfer logic properly to increase the hit rate
      ├──Memory.csv                 # Read/write bandwidth rates of UB, L1, and main memory
      ├──MemoryL0.csv               # Read/write bandwidth rates of L0A, L0B, and L0C
      ├──MemoryUB.csv               # Read/write bandwidth rates from Vector and Scalar to UB
      ├──OpBasicInfo.csv            # Basic operator information
      ├──PipeUtilization.csv        # Durations and proportions of computation and data transfer units
      ├──ResourceConflictRatio.csv  # Proportions of UB bank groups, bank conflicts, and resource conflicts among all instructions
      └──visualize_data.bin         # MindStudio Insight presentation file
      ```

View the specific performance analysis results:

```bash
# View Task Duration and various metrics
cat ./OPPROF_*/PipeUtilization.csv
```

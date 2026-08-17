# Gelu+Element-wise High-Performance Optimization Example

## Overview

This example uses Gelu+Element-wise computation (introducing a series of Element-wise computation operations after Gelu to increase computation depth) to introduce RegBase vector performance tuning methods. The example demonstrates performance benefits from parallelism adjustment, loop splitting, and loop unrolling.

> **Recommendation**: Before reading this example, it is recommended to first read the [Gelu Performance Optimization Example](../gelu_high_performance/README_en.md) to understand the basic VF fusion optimization principles, then learn the more complex loop splitting optimization methods in this example.

**Optimization Path**:
- Case 0: Single loop computation, parallelism adjusted to 128 bytes
- Case 1: Single loop computation, parallelism increased to 256 bytes
- Case 2: Split VF loop into two loops, executing Gelu and Element-wise computation separately
- Case 3: Based on Case 2, enable unroll loop unrolling optimization

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── gelu_eltwise_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Ground truth comparison script
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── gelu_eltwise.asc        // Ascend C example implementation (including 4 optimization cases)
│   └── README.md               // Example documentation
```

## Example Description

**Example Function**:

This example includes two computation steps: Gelu computation and Element-wise computation.

**Gelu Approximation Formula**:

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot (x + 0.044715 \cdot x^3)}}
$$

Where $-2 \cdot \sqrt{\frac{2}{\pi}} \approx -1.595769$, and $0.044715$ is the Gelu approximation formula coefficient.

**Element-wise Computation Formula**:

Based on Gelu output, perform a series of Element-wise computations:

$$
y = e^{-0.5 \cdot (GELU(x) + 1)^2 + 2}
$$

**Example Specifications**:

<table border="2">
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Gelu+Element-wise</td></tr>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[8192, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[8192, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">gelu_eltwise_custom</td></tr>
</table>

## Example Implementation

### Performance Metric Description

**Table 1 AI Core Performance Metric Field Description**

| Field Name | Field Meaning |
|:---|:---|
|Task Duration(μs)|Total Task duration, including scheduling time to the accelerator, execution time on the accelerator, and completion response time.|
|aiv_time(μs)|Task execution time on AI Vector Core.|
|aiv_total_cycles|Total cycle count computed across all vector cores.|
|aiv_vec_time(μs)|vec type instruction (vector computation instructions) duration.|
|aiv_vec_ratio|Ratio of vec type instruction cycle count to total cycle count.|
|aiv_scalar_time(μs)|scalar type instruction (scalar computation instructions) duration.|
|aiv_scalar_ratio|Ratio of scalar type instruction cycle count to total cycle count.|
|aiv_mte2_time(μs)|mte2 type instruction (GM->UB transfer instructions) duration.|
|aiv_mte2_ratio|Ratio of mte2 type instruction cycle count to total cycle count.|
|aiv_mte3_time(μs)|mte3 type instruction (UB->GM transfer instructions) duration.|
|aiv_mte3_ratio|Ratio of mte3 type instruction cycle count to total cycle count.|

### Case 0: Single Loop Computation, Parallelism Adjusted to 128 Bytes

**Implementation**: Refer to `KernelGeluEltwise::GeluEltwiseBasic()` function implementation

The baseline program uses RegBase API to implement Gelu and Element-wise computation. Following the principle of merging as much as possible, both computations are placed in the same VF loop; for comparison, the parallelism per iteration is set to 128 bytes (standard computation parallelism is 256 bytes).

**Key Code**:
```cpp
__simd_vf__ inline static void GeluEltwiseBasic(
    __ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum, uint32_t oneRepeatSize)
{
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;

    for (uint16_t i = 0; i < loopNum; ++i) {
        // Case 0 (scenarioNum == 0) uses a fixed VL32 mask (32 float elements, 128-byte parallelism);
        // only non-Case 0 uses UpdateMask<float>(n) to update the mask based on the remaining element count
        mask = AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::VL32>();
        // Gelu computation
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_A, mask);
        AscendC::Reg::Add(yReg, xReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_B, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);
        AscendC::Reg::Div(yReg, xReg, yReg, mask);
        // Element-wise computation
        AscendC::Reg::Adds(yReg, yReg, COEFF_C, mask);
        AscendC::Reg::Mul(yReg, yReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_D, mask);
        AscendC::Reg::Adds(yReg, yReg, COEFF_E, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);
    }
}
```

**Example Configuration**:
- Multi-core splitting: M direction split into 32 parts, N direction split into 2 parts, totaling 64 data portions distributed across 64 cores
- `tileLen = 8192` is the number of data elements per transfer and computation
- `oneRepeatSize = 128 / sizeof(float) = 32` elements

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 334.437 | 333.47 | 34582356 | 244.299 | 0.733 | 3.108 | 0.009 | 275.719 | 0.827 | 252.305 | 0.757 |

**Optimization Effect Analysis**:
- End-to-end duration: **334.437μs**
- Vector instruction duration: 244.299μs, ratio **73.3%**
- Data transfer duration: 275.719μs (read) + 252.305μs (write), transfer ratio exceeds **80%**
- Total computation cycle count: 34582356

**Next Optimization Direction**:
- Increase parallelism to 256 bytes to improve data processing efficiency

---

### Case 1: Single Loop Computation, Parallelism Increased to 256 Bytes

**Implementation**: Refer to `KernelGeluEltwise::GeluEltwiseBasic()` function implementation

Same implementation as Case 0, but parallelism is increased to 256 bytes (default value).

**Example Configuration**:
- Multi-core splitting: M direction split into 32 parts, N direction split into 2 parts, totaling 64 data portions distributed across 64 cores
- `tileLen = 8192` is the number of data elements per transfer and computation
- `oneRepeatSize = 256 / sizeof(float) = 64` elements

**Optimization Method**:
- **Parallelism Improvement**:
  - From 128 bytes to 256 bytes, improving single instruction computation throughput

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 324.862 | 324.09 | 33758796 | 125.203 | 0.386 | 3.187 | 0.01 | 292.819 | 0.904 | 280.044 | 0.864 |

**Optimization Effect Analysis**:
- End-to-end duration: **324.862μs**, compared to Case 0 duration reduced by **2.9%**
- Vector instruction duration: 125.203μs, compared to Case 0 duration reduced by **48.7%**
- After parallelism improvement, computation instruction throughput increases, vector computation efficiency improves significantly

**Next Optimization Direction**:
- Through analysis, IPC is significantly lower than the Gelu example (detailed calculation in later sections). Split the loop to shorten instruction computation depth in a single loop and improve instruction dual-issue efficiency

---

### Case 2: Split VF Loop into Two Loops

**Implementation**: Refer to `KernelGeluEltwise::GeluEltwiseLoopSplit()` function implementation

Split Gelu and Element-wise computation into two independent loops, executing steps 1-8 and steps 9-13 respectively.

**Key Code**:
```cpp
__simd_vf__ inline static void GeluEltwiseLoopSplit(
    __ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum, uint32_t oneRepeatSize)
{
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;
    uint32_t n2 = n;

    // Loop 1: Gelu computation
    for (uint16_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(n);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_A, mask);
        AscendC::Reg::Add(yReg, xReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_B, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);
        AscendC::Reg::Div(yReg, xReg, yReg, mask);
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);
    }

    // Loop 2: Element-wise computation
    for (uint16_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(n2);
        AscendC::Reg::LoadAlign(yReg, yAddr + i * oneRepeatSize);
        AscendC::Reg::Adds(yReg, yReg, COEFF_C, mask);
        AscendC::Reg::Mul(yReg, yReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_D, mask);
        AscendC::Reg::Adds(yReg, yReg, COEFF_E, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);
    }
}
```

**Example Configuration**:
- Multi-core splitting: M direction split into 32 parts, N direction split into 2 parts, totaling 64 data portions distributed across 64 cores
- `tileLen = 8192` is the number of data elements per transfer and computation
- `oneRepeatSize = 256 / sizeof(float) = 64` elements

**Optimization Method**:
- **Loop Splitting Principle**:
  - Split the 13-step computation into two loops: Gelu (8 steps) and Element-wise (5 steps)
  - In the VF function, a single loop has large instruction computation depth, but the execution queue depth is limited. The execution queue cannot hold more instructions, preventing maximum utilization of instruction dual-issue capability

- **Loop Splitting Benefit Analysis**:
  - Split point selection: Recommend choosing reduce operation endpoints or long-latency instruction (such as Exp, Div) endpoints
  - Gelu computation ends with Div, Element-wise ends with Exp, both are long-latency instructions
  - After splitting, each loop has shorter instruction dependency chains, benefiting instruction dual-issue

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 324.039 | 323.32 | 33794573 | 97.184 | 0.301 | 3.21 | 0.01 | 292.68 | 0.905 | 279.178 | 0.863 |

**Optimization Effect Analysis**:
- End-to-end duration: **324.039μs**, compared to Case 1 duration reduced by **0.25%**
- Vector instruction duration: 97.184μs, compared to Case 1 duration reduced by **22.7%**
- After loop splitting, instruction dependency chains are shortened, instruction dual-issue efficiency improves, vector computation efficiency further increases

**Next Optimization Direction**:
- Enable loop unrolling optimization to further improve instruction dispatch parallelism

---

### Case 3: Enable Unroll Loop Unrolling Optimization

**Implementation**: Refer to `KernelGeluEltwise::GeluEltwiseLoopSplit()` function implementation, adding `#pragma unroll 6` loop unrolling optimization

Based on Case 2, add unroll optimization to both loops separately.

**Key Code**:
```cpp
__simd_vf__ inline static void GeluEltwiseLoopSplit(
    __ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum, uint32_t oneRepeatSize)
{
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;
    uint32_t n2 = n;

    #pragma unroll 6
    // Loop 1: Gelu computation
    for (uint16_t i = 0; i < loopNum; ++i) {
        // ...
    }

    #pragma unroll 6
    // Loop 2: Element-wise computation
    for (uint16_t i = 0; i < loopNum; ++i) {
        // ...
    }
}
```

**Example Configuration**:
- Multi-core splitting: M direction split into 32 parts, N direction split into 2 parts, totaling 64 data portions distributed across 64 cores
- `tileLen = 8192` is the number of data elements per transfer and computation
- `oneRepeatSize = 256 / sizeof(float) = 64` elements

**Optimization Method**:
- **Loop Unrolling Principle**:
  - Through `#pragma unroll 6`, instruct the compiler to unroll the loop, 6 iterations per unroll
  - Improve instruction-level parallelism, enabling more VF instructions to be dispatched consecutively

- **Loop Unrolling Benefit Analysis**:
  - Unroll factor selection: `unroll 6` is an empirical value that needs tuning based on actual scenarios
  - Excessive unrolling: May increase register pressure, causing performance degradation, and also increases code size
  - Insufficient unrolling: Optimization effect is not obvious
  - Recommendation: Users can try incrementally to find the optimal loop unroll count

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 324.748 | 323.97 | 33905446 | 94.339 | 0.291 | 4.932 | 0.015 | 288.144 | 0.889 | 279.082 | 0.861 |

**Optimization Effect Analysis**:
- End-to-end duration: **324.748μs**, compared to Case 2 duration increased by **0.22%**
- Vector instruction duration: 94.339μs, compared to Case 2 duration reduced by **2.9%**
- After loop unrolling, more instructions can execute in parallel, vector computation efficiency slightly improves

---

## Performance Comparison Summary

### Ascend 950PR Performance Comparison

The table below shows performance data comparison for this example running on Ascend 950 Series Products:

| Case | Optimization Strategy | Cores | tileLen | Task Duration(μs) | aiv_vec_time(μs) | End-to-End Duration vs Case 0 | Vector Duration vs Case 0 |
|:---|:---|:---|:---|:---|:---|:---|:---|
| 0 | Single loop, parallelism 128 bytes (baseline) | 64 | 8192 | 334.437 | 244.299 | 1x | 1x |
| 1 | Single loop, parallelism 256 bytes | 64 | 8192 | 324.862 | 125.203 | 1.03x | 1.95x |
| 2 | Loop split (Gelu + Element-wise) | 64 | 8192 | 324.039 | 97.184 | 1.03x | 2.51x |
| 3 | Loop split + unroll loop unrolling | 64 | 8192 | 324.748 | 94.339 | 1.03x | 2.58x |

> **Notice:** This example is MTE2 bound, with the performance bottleneck being data transfer. End-to-end duration changes are small, but vector computation efficiency improves significantly.

### Optimization Summary

| Optimization Method | Core Principle | Usage Recommendation |
|:---|:---|:---|
| Parallelism adjustment | Maximize utilization of instruction parallel processing capability | Use full instruction computation parallelism |
| Loop splitting | Shorten instruction computation depth in a single loop, improve instruction dual-issue efficiency | Choose reduce endpoints or long-latency instruction endpoints as split points |
| Loop unrolling | Improve instruction dispatch parallelism, reduce loop control overhead | Use `#pragma unroll N`, N needs tuning based on actual scenario |

### Theoretical Performance Analysis

The table below shows the vector computation instruction parallelism for this example:

**Gelu Computation Instruction Parallelism**:

| No. | Instruction | Data Type | Computation Parallelism (bytes/cycle) |
|:---:|:---:|:---:|:---:|
| 1 | Mul | float | 256 |
| 2 | Mul | float | 256 |
| 3 | Muls | float | 256 |
| 4 | Add | float | 256 |
| 5 | Muls | float | 256 |
| 6 | Exp | float | 64 |
| 7 | Adds | float | 256 |
| 8 | Div | float | 64 |

**Element-wise Computation Instruction Parallelism**:

| No. | Instruction | Data Type | Computation Parallelism (bytes/cycle) |
|:---:|:---:|:---:|:---:|
| 9 | Adds | float | 256 |
| 10 | Mul | float | 256 |
| 11 | Muls | float | 256 |
| 12 | Adds | float | 256 |
| 13 | Exp | float | 64 |

**Theoretical vector duration calculation (without considering VF fusion instruction dual-issue feature)**:

This example includes 13 computation steps with the following instruction parallelism:
- 10 linear operation instructions (Mul×3, Muls×3, Add×1, Adds×3): parallelism 256 bytes/cycle
- 3 non-linear operation instructions (Exp×2, Div×1): parallelism 64 bytes/cycle

Cycles needed to process 1 byte of data:

$$
Cycles_{\text{per\_byte}} = 10 \times \frac{1}{256} + 3 \times \frac{1}{64} = \frac{10}{256} + \frac{12}{256} = \frac{22}{256}
$$

Equivalent data processed per cycle:

$$
P_{\text{bytes}} = \frac{256}{22} \approx 11.64 \text{ bytes/cycle}
$$

This example runs on Ascend 950PR Series Products, hardware parameters:
- Clock frequency $f = 1650 \text{ MHz}$
- AIV core count $N_{\text{core}} = 64$
- Data shape $M = N = 8192$, total data volume $D = M \times N \times 4 = 268435456 \text{ bytes}$

Theoretical vector duration formula:

$$
T_{\text{theory}} = \frac{D}{P_{\text{bytes}} \times f \times N_{\text{core}}}
$$

Substituting values:

$$
T_{\text{theory}} = \frac{268435456}{\frac{256}{22} \times 1650 \times 10^6 \times 64} \approx 219.0 \text{ μs}
$$

**IPC Analysis in VF Fusion Scenario**:

In the VF fusion scenario, vector instructions have dual-issue characteristics. IPC (Instructions Per Cycle) is used to measure performance.

IPC formula:

$$
IPC = \frac{N_{\text{instr}}}{Cycles_{\text{vec}}}
$$

Where:
- $N_{\text{instr}}$: Single core vector computation instruction count
- $Cycles_{\text{vec}} = T_{\text{vec}} \times f$: Single core vector computation cycle count
- $f = 1650 \text{ MHz}$: Hardware clock frequency

**Case 0: Single loop, parallelism 128 bytes**

Single core vector computation instruction count:

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 256 \times 13 = 425984
$$

Where:
- $N_{\text{VF}} = 128$: Number of VF function calls (outer Process loop)
- $N_{\text{loop}} = 256$: Number of loops within VF (when oneRepeatSize=32)
- $N_{\text{op}} = 13$: Number of computation instructions in VF (Gelu×8 + Element-wise×5)

Vector computation cycle count:

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 244.299 \text{ μs} \times 1650 \text{ MHz} = 403093
$$

IPC:

$$
IPC = \frac{425984}{403093} \approx 1.06
$$

---

**Case 1: Single loop, parallelism 256 bytes**

Single core vector computation instruction count:

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 13 = 212992
$$

Where:
- $N_{\text{VF}} = 128$: Number of VF function calls
- $N_{\text{loop}} = 128$: Number of loops within VF (when oneRepeatSize=64)
- $N_{\text{op}} = 13$: Number of computation instructions in VF

Vector computation cycle count:

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 125.203 \text{ μs} \times 1650 \text{ MHz} = 206585
$$

IPC:

$$
IPC = \frac{212992}{206585} \approx 1.03
$$

**IPC Analysis**: Case 1 compared to Case 0, IPC slightly decreases (1.03 vs 1.06), but vector duration significantly decreases (48.7% reduction). Note that while IPC is generally used to measure VF performance, the prerequisite is to maximize instruction parallelism utilization.

---

**Case 2: Loop split (Gelu + Element-wise)**

Single core vector computation instruction count remains unchanged:

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 13 = 212992
$$

The parameters in the formula are the same as those in Case 1. Splitting the loop does not change the total instruction count.

Vector computation cycle count:

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 97.184 \text{ μs} \times 1650 \text{ MHz} = 160354
$$

IPC:

$$
IPC = \frac{212992}{160354} \approx 1.33
$$

**IPC Analysis**: Case 2 compared to Case 1, IPC increases from 1.03 to 1.33, a **29.1%** improvement. After loop splitting, instruction dependency chains are shortened, and instruction dual-issue efficiency improves significantly.

---

**Case 3: Loop split + unroll loop unrolling**

Single core vector computation instruction count remains unchanged:

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 13 = 212992
$$

The parameters in the formula are the same as those in Case 2. Loop unrolling does not change the total instruction count.

Vector computation cycle count:

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 94.339 \text{ μs} \times 1650 \text{ MHz} = 155659
$$

IPC:

$$
IPC = \frac{212992}{155659} \approx 1.37
$$

**IPC Analysis**: Case 3 compared to Case 2, IPC increases from 1.33 to 1.37, a **3.0%** improvement. After loop unrolling, more instructions can execute in parallel, further improving instruction dispatch efficiency.

---

### IPC Comparison Summary

| Case | Optimization Strategy | Vector Duration (μs) | IPC | IPC vs Case 0 |
|:---:|:---|:---:|:---:|:---:|
| 0 | Single loop, parallelism 128 bytes (baseline) | 244.299 | 1.06 | 1x |
| 1 | Single loop, parallelism 256 bytes | 125.203 | 1.03 | 0.97x |
| 2 | Loop split (Gelu + Element-wise) | 97.184 | 1.33 | 1.25x |
| 3 | Loop split + unroll loop unrolling | 94.339 | 1.37 | 1.29x |

**IPC Optimization Recommendations**:
- Higher IPC is better, theoretical limit approaches 2.0. In most cases, achieving 1.4~1.5 indicates relatively optimal performance
- Case 3 IPC is 1.37, already close to ideal performance level
- Loop splitting is an optimization method for improving IPC that requires analysis and experimentation based on actual VF computation depth

---

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Switch Case

  Specify the case to build through `-DSCENARIO_NUM=N` during cmake build. Case descriptions:
  - `0`: Single loop, parallelism 128 bytes
  - `1`: Single loop, parallelism 256 bytes
  - `2`: Loop split (Gelu + Element-wise)
  - `3`: Loop split + unroll loop unrolling

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  SCENARIO_NUM=0
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic correctness
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes or Cases. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`, `1`, `2`, `3` | Case number: 0=Single loop+128bytes, 1=Single loop+256bytes, 2=Loop split, 3=Loop split+unroll |

- Execution results

  The following execution result indicates that the accuracy comparison succeeded.
  ```bash
  error ratio: 0.0000, tolerance:0.0001
  test pass!
  ```

### Performance Analysis

### Introduction to the msOpProf Tool

`msOpProf` is a single-operator performance analysis tool. It offers two usage methods: `msopprof` and `msopprof simulator`. The tool helps users identify anomalies in operator memory, operator code, and operator instructions, enabling comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and different file types (executables or operator binary `.o` files).

- On-device performance collection

    On-device performance collection directly measures the execution time of an operator on an Ascend AI Processor. This method is suitable for quickly locating operator performance issues in an on-device environment.

    Run operator tuning on the executable demo with `msopprof`:

    ```
    msopprof ./demo
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
# View Task Duration and various data
cat ./OPPROF_*/PipeUtilization.csv
```

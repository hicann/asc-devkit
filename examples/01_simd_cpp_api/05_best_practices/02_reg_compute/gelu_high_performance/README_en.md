# Gelu Performance Tuning Example

## Overview

This example uses Gelu computation to introduce RegBase vector performance tuning methods. The example demonstrates performance benefits from enabling VF fusion and loop unrolling.

> **Prerequisite Reading**: [Gelu Operator Introduction Example](../../../00_introduction/04_reg_compute/gelu/README_en.md). This example performs performance optimization based on the introduction example. It is recommended to read the introduction example first to understand basic concepts.

**Optimization Path**:
- Case 0: Gelu without VF fusion enabled (baseline)
- Case 1: Enable RegBase API and VF fusion
- Case 2: Enable RegBase API, VF fusion, and loop unrolling optimization

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── gelu_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Ground truth comparison script
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── gelu.asc                // Ascend C example implementation (including 2 optimization cases)
│   └── README.md               // Example documentation
```

## Example Description

**Example Function**:

Gelu approximation formula computation:

$$
GELU(x) \approx 0.5 \cdot x \cdot \left(1 + \tanh\left(\sqrt{\frac{2}{\pi}} \cdot \left(x + 0.044715 \cdot x^3\right)\right)\right) \tag{1}
$$

The tanh formula is:

$$
\tanh(u) = \frac{e^{2u} - 1}{e^{2u} + 1} \tag{2}
$$

Where $u = \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)$.

Substituting the tanh formula into the Gelu formula and simplifying:

$$
GELU(x) \approx \frac{x}{1 + e^{-2 \cdot \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)}} \tag{3}
$$

Where $-2 \cdot \sqrt{\frac{2}{\pi}} \approx -1.595769$.

This example uses formula (3) for programming computation. When designing vector operators, users should consider simplifying the original computation to effectively reduce computation steps and memory usage.

## Computation Step Analysis

To illustrate the impact of formula simplification on operator performance, three different Gelu computation implementations are compared.

**Method 1:**

$$
GELU(x) \approx 0.5 \cdot x \cdot \left(1 + \frac{e^{2 \cdot \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)} - 1}{e^{2 \cdot \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)} + 1}\right)
$$

**Method 2:**

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot x - 0.071405 \cdot x^3}}
$$

**Method 3:**

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot (x + 0.044715 \cdot x^3)}}
$$

| Computation Method | Computation Instructions | Unified Buffer (UB) Memory Portions |
|:---|:---:|:---:|
| Method 1 | 13 | 5 |
| Method 2 | 8 | 3 |
| Method 3 | 8 | 2 |

Let input UB memory be xLocal, output UB memory be yLocal, and temporary UB memory be uniformly represented as tmp0, tmp1, tmp2, etc.

**Method 1 Computation Step Breakdown** (13 instructions total, requires 5 memory portions):

| Step | Computation | Instruction | Memory Usage |
|:---:|:---|:---:|:---|
| 1 | yLocal = x² | Mul | xLocal, yLocal |
| 2 | yLocal = x³ | Mul | xLocal, yLocal |
| 3 | yLocal = 0.044715 * x³ | Muls | yLocal |
| 4 | yLocal = x + 0.044715 * x³ | Add | xLocal, yLocal |
| 5 | yLocal = √(2/π) * (x + 0.044715 * x³) | Muls | yLocal |
| 6 | yLocal = 2u | Muls | yLocal |
| 7 | tmp0 = e^(2u) | Exp | tmp0, yLocal |
| 8 | tmp1 = e^(2u) - 1 | Adds | tmp1, yLocal |
| 9 | tmp2 = e^(2u) + 1 | Adds | tmp2, yLocal |
| 10 | tmp1 = tanh(u) = (e^(2u) - 1) / (e^(2u) + 1) | Div | tmp1, tmp2 |
| 11 | yLocal = 1 + tanh(u) | Adds | yLocal, tmp1 |
| 12 | yLocal = x * (1 + tanh(u)) | Mul | xLocal, yLocal |
| 13 | yLocal = 0.5 * x * (1 + tanh(u)) | Muls | yLocal |

**Method 2 Computation Step Breakdown** (8 instructions total, requires 3 memory portions):

| Step | Computation | Instruction | Memory Usage |
|:---:|:---|:---:|:---|
| 1 | yLocal = x² | Mul | xLocal, yLocal |
| 2 | yLocal = x³ | Mul | xLocal, yLocal |
| 3 | yLocal = -0.071405 * x³ | Muls | yLocal |
| 4 | tmp0 = -1.595769 * x | Muls | tmp0, xLocal |
| 5 | yLocal = -1.595769 * x + (-0.071405 * x³) | Add | yLocal, tmp0 |
| 6 | yLocal = e^(-1.595769 * x - 0.071405 * x³) | Exp | yLocal |
| 7 | yLocal = 1 + e^(...) | Adds | yLocal |
| 8 | yLocal = x / (1 + e^(...)) | Div | yLocal, xLocal |

**Method 3 Computation Step Breakdown** (8 instructions total, requires 2 memory portions):

| Step | Computation | Instruction | Memory Usage |
|:---:|:---|:---:|:---|
| 1 | yLocal = x² | Mul | yLocal, xLocal |
| 2 | yLocal = x³ | Mul | yLocal, xLocal |
| 3 | yLocal = 0.044715 * x³ | Muls | yLocal |
| 4 | yLocal = x + 0.044715 * x³ | Add | xLocal, yLocal |
| 5 | yLocal = -1.595769 * (x + 0.044715 * x³) | Muls | yLocal |
| 6 | yLocal = e^(-1.595769 * (x + 0.044715 * x³)) | Exp | yLocal |
| 7 | yLocal = 1 + e^(...) | Adds | yLocal |
| 8 | yLocal = x / (1 + e^(...)) | Div | yLocal, xLocal |

**Comparison Summary**:

- **Computation instruction count**: Methods 2 and 3 both reduce by **5 instructions** compared to Method 1
- **Memory portions**: Method 2 reduces by **2 memory portions** compared to Method 1, Method 3 reduces by **3 memory portions** compared to Method 1
- **Implementation choice**: Method 3 is the optimal implementation with the fewest computation instructions and lowest memory usage. This example uses Method 3 implementation.

**Example Specifications**:

<table border="2">
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Gelu</td></tr>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[8192, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">y</td><td align="center">[8192, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">gelu_custom</td></tr>
</table>

## Example Implementation

### Performance Metric Description

**Table 1 AI Core Performance Metric Field Description**

| Field Name | Field Meaning |
|:---|:---|
|Task Duration(μs)|Total Task duration, including scheduling time to the accelerator, execution time on the accelerator, and completion response time.|
|aiv_time(μs)|Task execution time on AI Vector Core.|
|aiv_vec_time(μs)|vec type instruction (vector computation instructions) duration.|
|aiv_vec_ratio|Ratio of vec type instruction cycle count to total cycle count.|
|aiv_scalar_time(μs)|scalar type instruction (scalar computation instructions) duration.|
|aiv_scalar_ratio|Ratio of scalar type instruction cycle count to total cycle count.|
|aiv_mte2_time(μs)|mte2 type instruction (GM->UB transfer instructions) duration.|
|aiv_mte2_ratio|Ratio of mte2 type instruction cycle count to total cycle count.|
|aiv_mte3_time(μs)|mte3 type instruction (UB->GM transfer instructions) duration.|
|aiv_mte3_ratio|Ratio of mte3 type instruction cycle count to total cycle count.|

### Case 0: Gelu Without VF Fusion

**Implementation**: Refer to `KernelGelu::GeluCompute()` function implementation

The baseline program uses Ascend C basic API to implement Gelu computation, including vector instructions such as Mul, Muls, Add, Exp, Adds, Div.

**Key Code**:
```cpp
__aicore__ inline void GeluCompute(
        const AscendC::LocalTensor<float>& xLocal, const AscendC::LocalTensor<float>& yLocal, uint32_t n)
{
    // yLocal = x * x = x²
    AscendC::Mul(yLocal, xLocal, xLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x² * x = x³
    AscendC::Mul(yLocal, yLocal, xLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x³ * 0.044715 = 0.044715 * x³
    AscendC::Muls(yLocal, yLocal, COEFF_A, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x + 0.044715 * x³
    AscendC::Add(yLocal, xLocal, yLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = -1.595769 * (x + 0.044715 * x³)
    AscendC::Muls(yLocal, yLocal, COEFF_B, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = e^(-1.595769 * (x + 0.044715 * x³))
    AscendC::Exp(yLocal, yLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = 1 + e^(-1.595769 * (x + 0.044715 * x³))
    AscendC::Adds(yLocal, yLocal, (float)1.0, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x / (1 + e^(-1.595769 * (x + 0.044715 * x³)))
    AscendC::Div(yLocal, xLocal, yLocal, n);
}
```

**Example Configuration**:
- Multi-core splitting: M direction (shape first dimension, row direction) split into 32 parts, N direction (shape second dimension, column direction) split into 2 parts, totaling 64 data portions distributed across 64 cores
- `tileLen = 8192` is the number of data elements per transfer and computation

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 351.525 | 350.64 | 147.895 | 0.422 | 9.513 | 0.027 | 320.283 | 0.913 | 303.647 | 0.866 |

**Optimization Effect Analysis**:
- End-to-end duration: **351.525μs**
- Vector instruction duration: 147.895μs, ratio **42.2%**
- Data transfer duration: 320.283μs (read) + 303.647μs (write), transfer ratio exceeds **90%**

**Principle Description**:
- The computation flow follows formula (3), including 8 vector operations using basic API for vector computation
- Each vector computation internally performs load -> compute -> store operations, with each computation requiring data exchange between Unified Buffer (UB) and registers

**Next Optimization Direction**:
- Enable RegBase API and VF fusion to reduce data exchange between UB and registers, completing final result generation within registers

---

## Intermediate Performance Optimization

### Case 1: Enable RegBase API and VF Fusion

### Performance Bottleneck Analysis

Based on Case 0 Profiling data, the core performance bottlenecks of the current introductory code are:

**Profiling Data Collection**:

Use the `msOpProf` tool to collect performance data for Case 0. Key metrics:

| Metric | Value | Ratio |
|:---|:---|:---|
| Task Duration | 351.525μs | - |
| aiv_vec_time | 147.895μs | 42.2% |
| aiv_scalar_time | 9.513μs | 2.7% |
| aiv_mte2_time | 320.283μs | 91.3% |
| aiv_mte3_time | 303.647μs | 86.6% |

**Core Bottleneck Interpretation**:

- **Pain Point 1: High vector computation overhead ratio**: Vector instruction duration is 147.895μs, ratio 42.2%. Each basic API vector computation internally performs load → compute → store operations, with data needing to be exchanged between UB and registers repeatedly. 8 vector computations produce 8 Load/Store overheads.
- **Pain Point 2: Low instruction-level parallelism**: Basic API requires `PipeBarrier<PIPE_V>()` synchronization between each vector instruction, unable to utilize VF fusion dual-issue feature. Scalar overhead ratio is 2.7%.

### Optimization Method Analysis and Implementation: RegBase API + VF Fusion

**MemBase vs RegBase Concept Comparison**:

| Dimension | MemBase (Basic API) | RegBase (VF Fusion API) |
|:---|:---|:---|
| Programming Interface | Compute API (for example, `Mul(yLocal, xLocal, xLocal, n)`) | Reg API (for example, `Reg::Mul(yReg, xReg, xReg, mask)`) |
| Data Residency | Implicit Load/Store per instruction, intermediate results written back to UB | Data explicitly loaded to registers and stays resident, intermediate results stored in registers |
| Synchronization Method | Requires `PipeBarrier<PIPE_V>()` synchronization between instructions | No explicit synchronization needed within VF function, hardware manages dependencies automatically |
| Invocation Method | Directly call Compute API | Call VF function through `asc_vf_call` |
| Applicable Scenarios | Simple computation, single-step or few-step operations | Multi-step fused computation, reducing UB read/write count |
| Performance Characteristics | Large per-step Load/Store overhead, low IPC | Only 1 Load + 1 Store, supports dual-issue, high IPC |

> **Selection Recommendation**: When computation steps are numerous (≥3 steps) and intermediate results do not need to be written back to UB, prioritize RegBase + VF fusion to significantly reduce UB read/write count and utilize dual-issue feature to improve IPC.

**Key RegBase API Concepts**:

| Keyword/Concept | Meaning | Detailed Documentation |
|:---|:---|:---|
| `__simd_vf__` | VF function declaration modifier, indicating that the function runs in the VF (Vector Function) execution domain | [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md) |
| `__ubuf__` | UB address-space qualifier, indicating that the pointer points to a UB memory region | [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md) |
| `RegTensor<T>` | Register-level Tensor object whose data resides in vector registers (unlike `LocalTensor`, whose data resides in UB) | [RegTensor](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/RegTensor.md) |
| `MaskReg` | Vector mask register that controls the number of elements participating in each computation | [MaskReg](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/MaskReg.md) |
| `LoadAlign` | Contiguous aligned transfer that loads data from UB into registers | [Contiguous Aligned Load](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬入/LoadAlign_continuous.md) |
| `StoreAlign` | Contiguous aligned transfer that writes data from registers back to UB | [Contiguous Aligned Store](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬出/StoreAlign_continuous.md) |
| `asc_vf_call` | VF function call entry point used to invoke a VF function from a kernel function | [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md) |
| `UpdateMask` | Updates the mask register based on the number of remaining elements | [UpdateMask](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/MaskReg.md) |

**Principle Analysis**:

RegBase API provides register-level vector computation interfaces. Combined with [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md) to call VF functions, VF fusion can be achieved. Core advantages of VF fusion:
- Within a VF function, after data is loaded from UB to registers, all intermediate computations are completed within registers, requiring only one [LoadAlign](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬入/LoadAlign_continuous.md) and one [StoreAlign](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬出/StoreAlign_continuous.md), eliminating Load/Store overhead for intermediate results
- Supports VF dual-issue feature, standard computation instruction parallelism can reach 512 bytes/cycle, instruction dispatch efficiency (IPC) is significantly improved

```
Basic API Mode:                    VF Fusion Mode:
Load → Mul → Store              LoadAlign → Mul
Load → Mul → Store                   → Mul
Load → Muls → Store                   → Muls
Load → Add → Store                    → Add
Load → Muls → Store                   → Muls
Load → Exp → Store                    → Exp
Load → Adds → Store                   → Adds
Load → Div → Store                    → Div → StoreAlign
(8 Load/Store)                     (1 Load/1 Store)
```

**Code Modification Points**:

Compared to the introductory Case 0 code, key modifications are:
- Replace 8 independent vector computations from basic API with VF function called by `asc_vf_call`
- Use `Reg::LoadAlign`/`Reg::StoreAlign` within VF function to replace repeated Load/Store
- Remove `PipeBarrier<PIPE_V>()` synchronization; no explicit synchronization needed within VF function

```cpp
// Case 0: Basic API, each computation requires Load/Store + PipeBarrier synchronization
AscendC::Mul(yLocal, xLocal, xLocal, n);
AscendC::PipeBarrier<PIPE_V>();
AscendC::Mul(yLocal, yLocal, xLocal, n);
AscendC::PipeBarrier<PIPE_V>();
// ... 8 computations total, 8 synchronizations

// Case 1: RegBase API + VF fusion, all computation completed within registers
__simd_vf__ inline void GeluVfBasic(__ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;

    for (uint16_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(n);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);  // Only one Load
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_A, mask);
        AscendC::Reg::Add(yReg, xReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_B, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);
        AscendC::Reg::Div(yReg, xReg, yReg, mask);
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);  // Only one Store
    }
}
```

**VF Function Invocation Context**:

The above VF function defines the GELU computation logic within registers. It is called through `asc_vf_call` in the kernel function. The invocation example is as follows:

```cpp
// Code snippet for calling VF function in the kernel function
constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
uint32_t loopNum = DivCeil(n, oneRepeatSize);
__ubuf__ float* xAddr = reinterpret_cast<__ubuf__ float*>(xLocal.GetPhyAddr());
__ubuf__ float* yAddr = reinterpret_cast<__ubuf__ float*>(yLocal.GetPhyAddr());
// Call VF function through asc_vf_call, passing UB address, element count n, and loop count
asc_vf_call<GeluVfBasic>(xAddr, yAddr, n, loopNum);
```

**Example Configuration**:
- Multi-core splitting: M direction split into 32 parts, N direction 2 parts, totaling 64 data portions distributed across 64 cores
- `tileLen = 8192` is the number of data elements per transfer and computation

### Optimization Effect Evaluation

**Performance Improvement Comparison**:

| Metric | Case 0 (Introductory) | Case 1 (Intermediate Optimization) | Improvement |
|:---|:---:|:---:|:---:|
| Task Duration(μs) | 351.525 | 348.868 | 0.76% |
| aiv_vec_time(μs) | 147.895 | 66.277 | **55.2%** |
| aiv_vec_ratio | 0.422 | 0.19 | - |
| aiv_scalar_time(μs) | 9.513 | 3.03 | **68.2%** |
| aiv_scalar_ratio | 0.027 | 0.009 | - |
| aiv_mte2_time(μs) | 320.283 | 320.543 | - |
| aiv_mte3_time(μs) | 303.647 | 314.547 | - |

> This example is MTE2 bound, with data transfer ratio exceeding 90%, so end-to-end benefit is not obvious, but vector computation duration optimization effect is significant.

**Current Stage Conclusion**:
- Through VF fusion, vector instruction duration decreases from 147.895μs to 66.277μs, a **55.2%** reduction, IPC reaches 1.20
- Current example is MTE2 bound, performance bottleneck remains in data transfer, VF computation efficiency still has room for improvement
- Within the VF function, GELU computation dependency chain is long, instruction dual-issue efficiency within the loop is not fully utilized, IPC is only 1.20, still far from theoretical limit 2.0, requiring further optimization of instruction-level parallelism

---

## Advanced Extreme Refinement

### Case 2: Enable RegBase API, VF Fusion, and Loop Unrolling Optimization

### Extreme Performance Target and Residual Bottleneck Deep Analysis

**Gap from Theoretical Peak**:

Case 1 IPC is 1.20, still far from VF fusion theoretical limit IPC=2.0. In VF fusion scenario, standard computation instruction parallelism can reach 512 bytes/cycle, but current loop execution method limits instruction dual-issue efficiency.

**Microarchitecture-level Profiling Analysis**:

Based on Case 1 performance data, residual bottlenecks are mainly:
- **Bottleneck 1: Long instruction dependency chain within VF function loop**: GELU computation includes 8 vector instructions (Mul×2, Muls×2, Add×1, Exp×1, Adds×1, Div×1), forming a long dependency chain. During loop execution, the compiler schedules too few loop iterations in the limited execution queue, resulting in insufficient dual-issue instructions per cycle, IPC only 1.20
- **Bottleneck 2: Loop control scalar overhead**: for loop variable updates and conditional judgment introduce scalar instructions, whose relative proportion increases after vector instruction duration is significantly shortened

### Extreme Refinement Method: Loop Unrolling Optimization

**Principle**:

The for loop within the VF function is a simple iteration structure. The compiler generates loop control instructions (compare, jump) sequentially by default, causing scalar overhead between loop iterations. Through `#pragma unroll N`, instruct the compiler to unroll the loop N times, eliminating loop control overhead while allowing VF instructions from multiple iterations to be interleaved, improving instruction-level parallelism (ILP) and enabling more instructions to be dual-issued consecutively.

```
Unrolled:                         After Unrolling:
Iter0: Load→Mul→...→Store       Iter0: Load→Mul→...→Store
  ↓ (loop control overhead)      Iter1: Load→Mul→...→Store
Iter1: Load→Mul→...→Store       Iter2: Load→Mul→...→Store
  ↓ (loop control overhead)      Iter3: Load→Mul→...→Store
Iter2: Load→Mul→...→Store       ...(no loop control overhead, instructions can be interleaved)
```

**Optimization Plan**:

Add `#pragma unroll 6` before the for loop in the VF function. Unroll factor 6 is an empirical value that needs tuning based on actual scenarios:

```cpp
__simd_vf__ inline static void GeluVfBasic(__ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;
    #pragma unroll 6  // Loop unrolling optimization, improve instruction-level parallelism
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
}
```

**Unroll Factor Selection Recommendations**:
- Excessive unrolling: Increases register pressure, may cause spilling, performance may degrade, and code size increases
- Insufficient unrolling: Optimization effect is not obvious
- Recommendation: Try 2, 4, 6, 8 incrementally to find the optimal unroll count

**Example Configuration**:
- Multi-core splitting: M direction split into 32 parts, N direction 2 parts, totaling 64 data portions distributed across 64 cores
- `tileLen = 8192` is the number of data elements per transfer and computation

### Ultimate Performance Achievement Verification

**Profiling Report**:

| Metric | Case 0 (Baseline) | Case 1 (VF Fusion) | Case 2 (VF Fusion+Loop Unrolling) |
|:---|:---:|:---:|:---:|
| Task Duration(μs) | 351.525 | 348.868 | **344.436** |
| aiv_vec_time(μs) | 147.895 | 66.277 | **63.655** |
| aiv_vec_ratio | 0.422 | 0.19 | 0.185 |
| aiv_scalar_time(μs) | 9.513 | 3.03 | 4.757 |
| aiv_scalar_ratio | 0.027 | 0.009 | 0.014 |
| aiv_mte2_time(μs) | 320.283 | 320.543 | 315.468 |
| aiv_mte3_time(μs) | 303.647 | 314.547 | 306.069 |
| IPC | - | 1.20 | **1.25** |

**Key Verification Metrics**:
- Vector instruction duration decreases from Case 0's 147.895μs to **63.655μs**, a **56.9%** reduction
- IPC increases from Case 1's 1.20 to **1.25**, a **4.2%** improvement, instruction dispatch efficiency further approaches theoretical limit
- Loop unrolling effectively improves instruction-level parallelism, VF instructions from multiple iterations can be interleaved for dual-issue

**Ultimate Performance Summary**:
- With VF fusion + loop unrolling combined optimization, vector computation efficiency has reached a relatively good level
- Current IPC is 1.25, still has room from theoretical limit 2.0, mainly limited by GELU's 8-instruction dependency chain length
- This example is MTE2 bound (data transfer ratio exceeds 90%), end-to-end benefit is limited by transfer bottleneck
- Direction for further IPC improvement: Split overly long loops into multiple short loops at long-latency instruction endpoints (such as Exp, Div), making each dependency chain shorter and dual-issue efficiency higher. The loop in this example is short and not applicable to this optimization

---

## Performance Comparison Summary

### Ascend 950PR Performance Comparison

The table below shows performance data comparison for this example running on Ascend 950 Series Products:

| Case | Optimization Strategy | Cores | tileLen | Task Duration(μs) | aiv_vec_time(μs) | Theoretical Vector Duration (μs) | End-to-End Duration vs Case 0 | Vector Duration vs Case 0 |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 0 | Gelu without VF fusion (baseline) | 64 | 8192 | 351.525 | 147.895 | 139.02 | 1x | 1x |
| 1 | Enable RegBase API and VF fusion | 64 | 8192 | 348.868 | 66.277 | NA | 1.01x | 2.23x |
| 2 | Enable RegBase API, VF fusion and loop unrolling | 64 | 8192 | 344.436 | 63.655 | NA | 1.02x | 2.32x |

> **Notice:** This example is MTE2 bound, with the performance bottleneck being data transfer. The following analysis primarily focuses on vector duration calculation to help users analyze the performance benefits of enabling VF fusion and loop unrolling. Case 2 compared to Case 1, vector duration reduced by **4.6%**.

### Theoretical Performance Analysis

The table below shows the vector computation instruction parallelism for this example:

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

**Theoretical vector duration calculation**:

**Step 1: Calculate Gelu computation flow equivalent data processed per cycle (without considering VF fusion dual-issue feature)**

Gelu computation includes 8 vector instructions with the following parallelism:
- 6 linear operation instructions (Mul×2, Muls×2, Add×1, Adds×1): parallelism 256 bytes/cycle
- 2 non-linear operation instructions (Exp×1, Div×1): parallelism 64 bytes/cycle

Cycles needed to process 1 byte of data:
- Linear operation instructions: each needs 1/256 cycles
- Non-linear operation instructions: each needs 1/64 cycles

Total cycles needed to process 1 byte:

$$
Cycles_{\text{per\_byte}} = 6 \times \frac{1}{256} + 2 \times \frac{1}{64} = \frac{6}{256} + \frac{2}{64} = \frac{14}{256}
$$

Equivalent data processed per cycle:

$$
P_{\text{bytes}} = \frac{256}{14} = \frac{128}{7} \approx 18.29 \text{ bytes/cycle}
$$

**Step 2: Calculate theoretical vector duration (without considering VF fusion dual-issue feature)**

This example runs on Ascend 950PR Series Products, hardware parameters:
- Clock frequency $f = 1650 \text{ MHz} = 1.65 \times 10^9 \text{ Hz}$
- AIV core count $N_{\text{core}} = 64$
- Data shape $M = N = 8192$, total data volume $D = M \times N \times 4 = 268435456 \text{ bytes}$

Theoretical vector duration formula:

$$
T_{\text{theory}} = \frac{D}{P_{\text{bytes}} \times f \times N_{\text{core}}}
$$

Substituting values:

$$
T_{\text{theory}} = \frac{268435456}{\frac{128}{7} \times 1.65 \times 10^9 \times 64} = \frac{268435456 \times 7}{128 \times 1.056 \times 10^{11}} = \frac{1879048192}{1.35168 \times 10^{11}} \approx 139.02 \times 10^{-6} \text{ s} = 139.02 \text{ μs}
$$

**Step 3: Calculate theoretical vector duration in VF fusion scenario**

In VF fusion scenario, vector instructions have dual-issue characteristics, meaning standard computation instruction parallelism can reach 512 bytes/cycle. However, exp and div cannot simply be estimated as 128 bytes/cycle in dual-issue scenarios. In VF fusion scenario, IPC (Instructions Per Cycle) is more commonly used to measure performance.

For Case 1 scenario, single core vector computation instruction count:

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 8 = 131072
$$

Where:
- $N_{\text{VF}} = 128$: Number of VF function calls
- $N_{\text{loop}} = 128$: Number of loops within VF
- $N_{\text{op}} = 8$: Number of computation instructions in VF (Mul×2, Muls×2, Add×1, Adds×1, Exp×1, Div×1)

Vector computation cycle count:

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 66.277 \text{ μs} \times 1650 \text{ MHz} = 109357
$$

Where:
- $T_{\text{vec}} = 66.277 \text{ μs}$: Measured vector computation duration
- $f = 1650 \text{ MHz}$: Hardware clock frequency

IPC:

$$
IPC = \frac{N_{\text{instr}}}{Cycles_{\text{vec}}} = \frac{131072}{109357} \approx 1.20
$$

Where:
- $N_{\text{instr}} = 131072$: Single core vector computation instruction count
- $Cycles_{\text{vec}} = 109357$: Vector computation cycle count

IPC is better when higher, theoretical limit approaches 2.

**Case 2: IPC Analysis After Loop Unrolling Optimization**

For Case 2 scenario, with loop unrolling optimization added on top of VF fusion, single core vector computation instruction count remains unchanged:

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 8 = 131072
$$

The parameters in the formula have the same meanings as in Case 1. Loop unrolling does not affect the total instruction count.

Vector computation cycle count:

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 63.655 \text{ μs} \times 1650 \text{ MHz} = 105031
$$

IPC:

$$
IPC = \frac{N_{\text{instr}}}{Cycles_{\text{vec}}} = \frac{131072}{105031} \approx 1.25
$$

**Impact of Loop Unrolling on IPC Analysis**:
- Case 2 IPC is 1.25, compared to Case 1 IPC (1.20) improved by **4.2%**
- Loop unrolling optimization improves vector instruction dispatch efficiency, enabling more instructions to execute in parallel

**IPC Optimization Recommendations**:
- In VF fusion scenario, IPC is an important metric for measuring vector instruction dispatch efficiency
- Ideal IPC limit approaches 2.0. In most cases, achieving 1.4~1.5 indicates relatively optimal performance
- When a VF function loop contains an overly long instruction dependency chain, the execution queue holds too few loop iterations, resulting in fewer dual-issue instructions per cycle and performance degradation. In this case, overly long loops can be split into multiple shorter loops, for example at reduce endpoints or long-latency instruction endpoints (such as div, exp), splitting one loop into 2~3 loops. The example loop in this case is short and not applicable to this optimization.

### Optimization Summary

| Optimization Method | Core Principle | Usage Recommendation |
|:---|:---|:---|
| Formula simplification | Reduce computation steps, lower computation overhead | Prioritize formula derivation and simplification |
| RegBase API + VF fusion | Register-level computation reduces intermediate Load/Store, utilizes dual-issue feature to improve performance | Use asc_vf_call to invoke VF functions, utilize dual-issue feature to improve IPC |
| Loop unrolling | Improve instruction dispatch parallelism | Use `#pragma unroll N`, N needs tuning based on actual scenario |

---

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Switch Case

  Specify the case to build through `-DSCENARIO_NUM=N` during cmake build. Case descriptions:
  - `0`: Gelu without VF fusion (requires setting `-DCMAKE_VF_MODE=false`)
  - `1`: Enable RegBase API and VF fusion
  - `2`: Enable RegBase API, VF fusion and loop unrolling optimization

  > **Notice:** The compiler has automatic VF fusion capability, enabled by default. For convenient performance comparison analysis in this example, automatic VF fusion needs to be disabled for case 0.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  SCENARIO_NUM=0
  CMAKE_VF_MODE=false
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_VF_MODE=$CMAKE_VF_MODE;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic correctness
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_VF_MODE=$CMAKE_VF_MODE;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes or Cases. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`, `1`, `2` | Case number: 0=Gelu without VF fusion, 1=Enable RegBase API and VF fusion, 2=Enable RegBase API, VF fusion and loop unrolling |
  | `CMAKE_VF_MODE` | `true`, `false` | VF fusion mode: for case 0, set to false to disable automatic VF fusion; for case 1/2, set to true to enable VF fusion |

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

# Softmax Performance Tuning Example

## Overview

This example uses single-core Softmax as a case study to demonstrate a complete performance tuning path from MemBase to RegBase, and from basic loops to optimizations such as loop fusion and loop unrolling. It includes six progressively optimized versions: Case 0 through Case 5.

**Optimization Path**:
- Case 0 → Case 1: MemBase → RegBase API (register-level computation)
- Case 1 → Case 2: Loop fusion + ExpSub fused instruction + UpdateMask tail block handling
- Case 2 → Case 3: Outer loop unrolling (UpdateMask mode)
- Case 2 → Case 4: Main-tail block mode
- Case 4 → Case 5: Main-tail block mode + Outer loop unrolling + ExpSub fused instruction

**Prerequisites**: Before reading, a basic understanding of the following background topics is recommended to provide theoretical support for the optimization strategies of each Case:
- **[Reg Vector Computation Programming](../../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/reg_vector_computation.md)**
- **[Instruction Dual-Issue Optimization](../../../../../docs/zh/guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/dual_issue_optimization.md)**
- **[VF Fusion Optimization](../../../../../docs/zh/guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_fusion_optimization.md)**
- **[VF Loop Optimization](../../../../../docs/zh/guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_loop_optimization.md)**

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|-----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── softmax_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth data generation script
│   │   └── verify_result.py    // Ground truth comparison script
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── softmax.asc             // Ascend C sample implementation (Case 0-5, six versions)
│   └── README.md               // Sample documentation
```

## Sample Description

**Sample Function**:

The sample implements Softmax with a fixed shape (m, n) of (128, 128).

The Softmax formula is:

$$
\text{Softmax}(x_{i,j}) = \frac{e^{x_{i,j} - \max_i}}{\sum_{j=0}^{n-1} e^{x_{i,j} - \max_i}}
$$

- x: input, shape [128, 128], data type float
- y: output, shape [128, 128], data type float

**Sample Specifications**:

<table border="2">
<tr><td rowspan="1" align="center">OpType</td><td colspan="4" align="center">Softmax</td></tr>
<tr><td rowspan="2" align="center">Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Output</td><td align="center">y</td><td align="center">[128, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Name</td><td colspan="4" align="center">softmax_custom</td></tr>
</table>

## Version Overview

**Table 1 Performance and Optimization Description for Each Version**
| Version | API Type | Characteristics | Performance (Converted Vector Instruction Cost) |
|:---:|:---|:---|:---:|
| **Case 0** | MemBase | Baseline version, all computation in a single loop | 44113 cycles |
| **Case 1** | RegBase | First use of Reg vector API, unfused, with redundant Load/Store/sync | 31894 cycles |
| **Case 2** | RegBase | UpdateMask mode + Loop fusion + ExpSub fused instruction | 3413 cycles |
| **Case 3** | RegBase | UpdateMask mode + Outer loop unrolling, improves dual-issue capability | 2197 cycles |
| **Case 4** | RegBase | Main-tail block mode, replaces UpdateMask | 3424 cycles |
| **Case 5** | RegBase | Main-tail block mode + Outer loop unrolling + ExpSub fused instruction (optimal) | 1785 cycles |

**Table 2: AI Core Performance Metric Field Descriptions**

| Field Name | Meaning |
|:---:|:---|
| aiv_total_cycle | End-to-end cycle count of the vector single operator. |
| aiv_vec_ratio | Ratio of vector-type instructions in end-to-end execution time. |

**Notes**:
1. This example focuses on VF performance optimization details within single-core Softmax. Factors such as multi-core partitioning, data movement, and DoubleBuffer are not considered here.
2. Vector instruction cost is calculated by multiplying aiv_total_cycles and aiv_vec_ratio.

## Optimization Strategy Analysis

This section details the progressive optimization strategies from Case 0 to Case 5.

## Phase 1: MemBase (Case 0)

### Case 0: MemBase Baseline Version

**Implementation**: Uses Ascend C basic API Memory vector computation interfaces. Within a 128-iteration for loop, all 7 steps (ReduceMax, Duplicate, Sub, Exp, ReduceSum, Duplicate, Div) are performed sequentially.

**Theoretical Analysis — Why MemBase Vector Computation Interfaces Are Slow**:

Case 0 uses Memory vector computation APIs. During computation, these APIs encapsulate Vector Function calls to Reg vector interfaces for calculation, achieving intergenerational compatibility. Each instruction's semantics represent the complete flow of "read source operands from UB into registers → compute in execution units → write register data back to UB destination operands." This means:

1. **Every intermediate result must be written back to UB**: After ReduceMax writes the result to UB, Duplicate must re-read from UB; after Duplicate writes to UB, Sub must re-read from UB... and so on. Intermediate results are repeatedly read from and written to UB, resulting in poor performance.
2. **Redundant synchronization and instructions**: Since data is repeatedly moved in and out of registers, there is no register-level data reuse. Data dependencies exist between Vector Functions, preventing dual-issue and other capabilities.
3. **High UB Bank conflict risk**: Continuous UB read/write access is highly likely to trigger Bank conflicts, increasing access latency.

**Key Code**:
```cpp
for (uint32_t i = 0; i < aDim; i++) {
    AscendC::ReduceMax(expTensor[i * rDimAlign], srcTensor[i * rDimAlign], sumTensor[i * rDimAlign], rDim);
    AscendC::Duplicate(expTensor[i * rDimAlign], expTensor[i * rDimAlign], rDim);
    AscendC::Sub(expTensor[i * rDimAlign], srcTensor[i * rDimAlign], expTensor[i * rDimAlign], rDim);
    AscendC::Exp(expTensor[i * rDimAlign], expTensor[i * rDimAlign], rDim);
    AscendC::ReduceSum(sumTensor[i * rDimAlign], expTensor[i * rDimAlign], dstTensor[i * rDimAlign].ReinterpretCast<float>(), rDim);
    AscendC::Duplicate(sumTensor[i * rDimAlign], sumTensor[i * rDimAlign], rDim);
    AscendC::Div(dstTensor[i * rDimAlign], expTensor[i * rDimAlign], sumTensor[i * rDimAlign], rDim);
}
```

**Performance**: 44113 cycles (vector portion)

**Note**: MemBase interfaces can benefit from compiler-level optimization by fusing Vector Functions across interfaces to reduce redundant data movement overhead. However, since the fusion process is not transparent, it is less flexible than the RegBase vector computation interface.

## Phase 2: MemBase → RegBase (Case 0 → Case 1)

### Case 1: Reg Vector API Baseline — Unfused Loops, Contains Redundant Operations

**Optimization Motivation**: MemBase APIs implicitly execute Load→Compute→Store for each vector computation, requiring a Vector Function launch per operation. RegBase APIs can load data into registers for persistent residency, keep intermediate results in registers, complete multiple computations via a single Vector Function, and only need a final Store back to UB.

**Theoretical Analysis — Key Benefits of MemBase → RegBase**:

1. **Register programming reduces redundant instructions and VF launch overhead**:
   In MemBase, a single interface corresponds to a single Vector Function. After RegBase introduces SIMD Registers, computations from multiple interfaces can be completed by launching a single Vector Function executing multiple instructions, thus reducing VF launch overhead:
   ```
   MemBase: UB → Load → Compute → Store → UB → Load → Compute → Store → UB ...
   RegBase: UB → Load → Compute(reg) → Compute(reg) → ... → Store → UB
   ```
   Note that the current RegBase baseline approach still writes back to UB without loop fusion.

2. **Explicit Load/Store control over data movement timing**:
   In MemBase, data movement actions are implicitly performed by the API and cannot be controlled by the developer. RegBase, through `LoadAlign` / `StoreAlign` (aligned movement) or `LoadUnAlign` / `StoreUnAlign` (unaligned movement), allows explicit control over movement granularity and timing, enabling precise scheduling of data flow.

**Key Changes**:

1. Uses `Reg::LoadAlign` / `Reg::StoreAlign` for explicit data movement control
2. Uses register-level APIs such as `Reg::Max` / `Reg::Sub` / `Reg::Exp` / `Reg::ReduceMax` / `Reg::ReduceSum`
3. Uses `UpdateMask` to handle unaligned tail blocks
4. Uses `LocalMemBar` to ensure data consistency

**Problems with Case 1**:
- Multiple loops are unfused, resulting in redundant Load/Store instructions
- `LocalMemBar` is inserted after each computation, introducing redundant synchronization
- ExpSub fused instruction is not used; Sub and Exp are executed separately

**Performance**: 31894 cycles (34% improvement over Case 0). Register-level computation brought by RegBase eliminates VF launch overhead, but since each computation is done in a separate loop and written back to UB, significant room for optimization remains.

## Phase 3: RegBase Deep Optimization (Case 1 → Case 2)

### Case 2: Loop Fusion + ExpSub Fused Instruction + UpdateMask Tail Block Handling

**Theoretical Analysis — Deep Principles of the Three Optimizations**:

**1. Loop fusion eliminates redundant Load/Store/synchronization**:

In Case 1, the ReduceMax loop and ExpSub loop each independently Load source data from UB — essentially performing the same UB→register movement twice. Loop fusion shares the Load between the two loops, thereby eliminating redundant data movement and synchronization instructions. The principle is as follows:

```
Case 1 (unfused):
  for i: for j: Load(src) → Max → ReduceMax     // Load src, 1st time
  LocalMemBar()
  for i: for j: Load(src) → Sub+Exp → Store      // Load src, 2nd time (redundant!)
  LocalMemBar()
  for i: for j: Load(exp) → Add → ReduceSum      // Load exp, 1st time
  LocalMemBar()
  for i: for j: Load(exp) → Duplicate+Div → Store // Load exp, 2nd time (redundant!)

Case 2 (fused):
  for i:
      for j: Load(src) → Max → ReduceMax          // Load src
      for j: Load(src) → ExpSub → Store           // Reuse already-loaded src (no reload needed)
  LocalMemBar()
  for i:
      for j: Load(exp) → Add → ReduceSum          // Load exp
      for j: Load(exp) → Duplicate+Div → Store    // Reuse already-loaded exp
```

Case 2 requires only 2 Loads (one for src, one for exp), while Case 1 requires 4 Loads. Fusion eliminates 2 redundant UB data movements and removes some synchronization.

**2. Instruction fusion improves performance**:

In Case 1, `Sub` and `Exp` are two independent instructions:
```cpp
// Case 1: Sub → Exp execute sequentially, Exp must wait for Sub result in dstReg
AscendC::Reg::Sub(dstReg, srcReg, maxReg, mask);
AscendC::Reg::Exp(dstReg, dstReg, mask);
```
After ExpSub fusion, they become a single instruction, eliminating the cost of one instruction. The larger the last dimension of the Softmax shape, the greater the benefit:
```cpp
// Case 2: One-step fusion, Sub→Exp completed internally in hardware
AscendC::Reg::ExpSub(expReg, srcReg, maxReg, mask);
```
In the dual-issue execution queue, removing one dependent instruction means subsequent independent instructions can be issued earlier.

**3. UpdateMask vs Main-Tail Block**:

UpdateMask mode calls `UpdateMask` to update the mask in each inner loop iteration. Although it satisfies Hardware Loop requirements (no if/else), each call still incurs scalar computation overhead. The main-tail block mode splits data into "main block (full Mask)" and "tail block (partial Mask)" for separate processing. The main block loop requires no mask update operations at all, completely eliminating the cumulative overhead of UpdateMask.

Using `UpdateMask` to automatically update the mask in each inner loop, producing concise code:
```cpp
uint32_t count = rDim;
for (uint16_t j = 0; j < repeatTimes; j++) {
    mask = AscendC::Reg::UpdateMask<float>(count);
    LoadWithCastIfNeed(srcAddr, srcReg, mask, ...);
    AscendC::Reg::Max(maxReg, maxReg, srcReg, mask);
}
```

**Result**: 31894 → 3413 cycles (9.3x improvement). The fusion benefit is significant.

## Phase 4: Loop Unrolling Optimization, Proper Use of Main-Tail Block Mode

### Case 2 → Case 3: Outer Loop Unrolling (UpdateMask Mode)

**Optimization Motivation**: In Case 2, 128 rows are processed sequentially, making cross-row instruction scheduling difficult and limiting VF dual-issue efficiency.

**Theoretical Analysis — How Outer Loop Unrolling Improves Dual-Issue**:

Case 2's execution pattern: row 0's Load→Max→ReduceMax→Load→ExpSub→Store is fully completed before starting row 1. At this point, the execution queue contains only the instruction sequence for a single row (row i), and within row i there is a clear dependency chain:

```
Load(src_i) → Max(max_i, src_i) → ... → ReduceMax(max_i) → ... → Load(src_i) → ExpSub(...) → Store
```

Instructions on the dependency chain must wait for predecessor instructions to complete and cannot be issued in parallel with any other instruction. Since all internal instructions depend on each other, the number of actually concurrently issuable dependency-free instructions is small.

**Outer loop unrolling (unroll factor = 2)** uses dual register sets (`*Reg` and `*Reg1`), processing two rows at a time:

```
Row 0: Load(src0) → Max(max0, src0) → ... → ReduceMax(max0) → Load(src0) → ExpSub(exp0) → Store
Row 1:   Load(src1) → Max(max1, src1) → ... → ReduceMax(max1) → Load(src1) → ExpSub(exp1) → Store
```

Row 0 and Row 1 have **no data dependencies** between them. The two pipelines can issue simultaneously: the execution queue is simultaneously filled with dependency-free instructions from two rows, significantly improving the dual-issue rate.

This mechanism corresponds to the core principle in "Instruction Dual-Issue Optimization": provide the execution queue with a sufficient number of concurrent, dependency-free instructions. With the current shape of 128 rows, each unroll group processes 2 rows, totaling 64 groups. Register usage is roughly 2x that of single-row mode, still within hardware limits (approximately 6-8 RegTensors after unrolling, far from the 32 upper limit).

**Optimization Approach**: Manually unroll the outermost loop, processing two rows at a time using dual register sets (`*Reg` and `*Reg1`):

```cpp
// Case 2: Sequential processing
for (uint16_t i = 0; i < aDim; i++) {
    // Single row ReduceMax → ExpSub
}
for (uint16_t i = 0; i < aDim; i++) {
    // Single row ReduceSum → Div
}

// Case 3: Outer loop unrolled, two rows at a time
uint16_t halfA = aDim >> 1;
for (uint16_t i = 0; i < halfA; i++) {
    Duplicate(maxReg, ...);  Duplicate(maxReg1, ...);
    for (j) {
        Load(srcRow0, srcRow0+halfA_offset);  // Interleaved Load for two rows
        Max(maxReg, srcRow0);  Max(maxReg1, srcRow1);
    }
    ReduceMax(maxReg);  ReduceMax(maxReg1);
    for (j) {
        Load(srcRow0, srcRow0+halfA_offset);
        ExpSub(expReg);  ExpSub(expReg1);
        Store(...);
    }
}
```

**Result**: 3413 → 2197 cycles (35.6% improvement). Outer loop unrolling effectively improves dual-issue capability.

### Case 2/3 → Case 4/5: Main-Tail Block Mode

**Optimization Motivation**: Although UpdateMask has automatic mask update characteristics, each UpdateMask call still introduces scalar computation (updating count values, generating masks). In main-tail block mode, all inner loops of the 128-row main block use the compile-time created `CreateMask<ALL>()` (full Mask), completely eliminating the cumulative overhead of UpdateMask. Within the loop, **no conditional checks or mask updates are needed**. Under the current shape with `rDim=128`, each float occupies 4B, `VL=256B` corresponds to 64 elements, which exactly divides `128 / 64 = 2`. The tail block is 0, and the main-tail block mode reaches its maximum benefit.

**Key Code Changes**:
```cpp
// First compute main block iterations and tail block size
uint16_t tail = rDim % (256 / sizeof(float));
uint16_t repeatTimesMain = rDim / (256 / sizeof(float));
uint16_t hasTail = (tail != 0);

// Case 4: Inner loops all use full mask (maskFull), no UpdateMask needed
for (uint16_t i = 0; i < aDim; i++) {
    for (uint16_t j = 0; j < repeatTimesMain; j++) {
        LoadWithCastIfNeed(srcAddr, srcReg, maskFull, ...);
        AscendC::Reg::Max(maxReg, maxReg, srcReg, maskFull);
    }
    ReduceMax(maxReg, maskFull);
    Duplicate(maxReg, maxReg, maskFull);
    for (uint16_t j = 0; j < repeatTimesMain; j++) {
        LoadWithCastIfNeed(srcAddr, srcReg, maskFull, ...);
        AscendC::Reg::ExpSub(expReg, srcReg, maxReg, maskFull);
    }
}
```

**Theoretical Analysis — Optimal Combination for Maximum Dual-Issue Efficiency**:

Case 5 simultaneously applies three mutually non-conflicting optimization techniques across three dimensions, achieving a "1 + 1 + 1 > 3" synergistic effect:

| Optimization | Core Benefit | Impact on Registers | Mutual Interference |
| --- | --- | --- | --- |
| Main-tail block split | Main block uses full Mask, eliminates UpdateMask overhead | No extra registers | No interference |
| Outer loop unrolling (×2) | Two rows interleaved issue, fill execution queue | ×2 RegTensor usage | No interference |
| ExpSub fused instruction | Sub+Exp merged into single instruction, shortens dependency chain | Reduces 1 temporary RegTensor | No interference |

Combination mechanism of the three optimizations:

1. **Register resource utilization**: Under the current shape, a single row requires about 4-6 RegTensors (src, max, exp, sum, temporary registers). After 2× unrolling, approximately 8-12 RegTensors are needed, with MaskReg usage around 2-3, all within hardware limits (≤32 RegTensor, ≤8 MaskReg). Sufficient register resources are the prerequisite for the three optimizations to be superimposed.

2. **Maximizing execution queue depth**: Outer loop unrolling fills the execution queue with dependency-free instructions from two rows; main-tail block ensures the main block loop has no conditional branches (full Mask, no UpdateMask calls), with a continuous, jump-free instruction sequence; ExpSub merges the two dependent instructions Sub+Exp into a single one, reducing the total number of instructions in the execution queue and shortening dependency chains.

**Performance**: 1785 cycles (27.2x improvement over Case 0), the optimal performance for this sample.

**Conclusion**: Under the current shape, the last axis is only 128 with a tail block of 0. The main-tail block mode with unrolling achieves optimal performance (Case 5). Correspondingly, code complexity increases. The UpdateMask mode produces more concise code, so a trade-off must be made based on the scenario.

## Performance Comparison Summary

### Optimization Highlights Summary

| Optimization | Core Principle | Applicable Versions |
|:---|:---|:---|
| MemBase→RegBase | Register-level computation reduces UB↔register round trips and VF launch overhead | Case 0→Case 1 |
| ExpSub fused instruction | Single instruction replaces Sub+Exp, reduces latency | Case 1→Case 2 |
| Loop fusion | Shared Load/Store, eliminates redundant movement and synchronization | Case 1→Case 2 |
| UpdateMask | Automatic mask update, concise tail block handling | Case 2, Case 3 |
| Main-tail block mode | Main block full Mask with no update overhead, tail block handled separately | Case 4, Case 5 |
| Outer loop unrolling | Multiple register sets interleaved execution, improves dual-issue | Case 2→Case 3, Case 4→Case 5 |

## Build and Run

Execute the following steps in the sample's root directory to build and run the sample.

- Select version

  Use `-DSCENARIO_NUM=N` (N=0~5) to specify the version to compile:

  | SCENARIO_NUM | Version | Description | CMAKE_VF_MODE |
  |:---:|:---:|:---|:---:|
  | 0 | Case 0 | MemBase, single loop | false |
  | 1 | Case 1 | RegBase, unfused | false |
  | 2 | Case 2 | RegBase, UpdateMask+ExpSub+Fusion | false |
  | 3 | Case 3 | RegBase, UpdateMask+Loop unrolling | false |
  | 4 | Case 4 | RegBase, Main-tail block mode | false |
  | 5 | Case 5 | RegBase, Main-tail block+Loop unrolling+ExpSub | false |

- Configure environment variables
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

- Single version execution
  ```bash
  mkdir -p build && cd build
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=5 -DCMAKE_VF_MODE=false
  make -j
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

- Execution result

  ```bash
  error ratio: 0.0000, tolerance:0.0001
  test pass!
  ```

- Build option descriptions

  | Option | Values | Description |
  |:---|:---|:---|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture |
  | `SCENARIO_NUM` | `0`~`5` | Version number Case 0-5 |
  | `CMAKE_VF_MODE` | `true`, `false` | VF fusion mode: Default is false |

### Performance Analysis

Use the `msOpProf` tool to obtain detailed performance data:

```bash
msopprof ./demo
cat ./OPPROF_*/PipeUtilization.csv
```

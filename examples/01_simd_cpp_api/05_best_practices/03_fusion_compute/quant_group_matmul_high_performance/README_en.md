# QuantGroupMatmul Best Practice Example

## Overview

This example implements per-token quantization group matrix multiplication (QuantGroupMatmul) and demonstrates the performance tuning method in the Vector bound scenario during CV fusion.

## Products and CANN software versions supported by this example

| Products | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 training series products/Atlas A3 inference series products | >= CANN 9.0.0 |
| Atlas A2 training series products/Atlas A2 inference series products | >= CANN 9.0.0 |

## Introduction to directory structure

```
├── quant_group_matmul_high_performance
│   ├── CMakeLists.txt                     // Compile project files
│   ├── data_utils.h                       // Data reading and writing functions
│   ├── figures                            // Image resource directory in README
│   ├── quant_group_matmul_custom.asc      // Ascend C sample implementation & calling sample
│   ├── README.md                          // Sample documentation
│   └── scripts
│       ├── gen_data.py                    // Input data and ground truth data generation script
│       └── verify_result.py               // Verification script to verify whether output data and true value data are consistent
```

## Sample description

- Sample functions:
  The example implements grouped per-token quantization matmul calculation, the grouping axis is the m-axis, and the activation function Gelu is calculated on the result.

  The calculation formula of QuantGroupMatmul is:

  $$
  s_i = \sum_{t=0}^{i-1} group[t]
  $$

  $$
  y[s_i:s_i + group[i], :] =
  \operatorname{Gelu}\left(
  \left(x[s_i:s_i + group[i], :] \times weight[i]\right)
  \odot scale[i] \odot perTokenScale[s_i:s_i + group[i]]
  \right), \quad 0 \le i < g
  $$

  Among them, $s_i$ represents the starting position of the $i$-th group on the m-axis, and $\odot$ represents element-by-element multiplication according to broadcast rules.

  - x: left matrix, shape is [m, k], data type is int8;
  - weight: right matrix, logical shape is [g, k, n], the actual input file is in NZ format, and the data type is int8;
  - group: record the size of each group m, the data type is int64;
  - scale: the quantization parameter of the right matrix, the shape is [g, n], the data type is float, used for inverse quantization of the matrix multiplication result, and scale[i] is used for inverse quantization of the i-th matrix multiplication result;
  - perTokenScale: per-token scale of the left matrix, shape is [m], data type is float, used for inverse quantization of the matrix multiplication result, using the same index range as the x row for inverse quantization;
  - y: output, a matrix that stores the result of matrix multiplication, the shape is [m, n], and the data type is float16;

- Sample specifications:

<table>
<tr><td rowspan="1" align="center">Sample Type (OpType)</td><td colspan="4" align="center">QuantGroupMatmul</td></tr>
<tr><td rowspan="6" align="center"> sample input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[8192, 1024]</td><td align="center">int8</td><td align="center">ND</td></tr>
<tr><td align="center">weight</td><td align="center">[8, 1024, 8192]</td><td align="center">int8</td><td align="center">NZ</td></tr>
<tr><td align="center">group</td><td align="center">[8]</td><td align="center">int64</td><td align="center">ND</td></tr>
<tr><td align="center">scale</td><td align="center">[8, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">perTokenScale</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center"> sample output</td><td align="center">y</td><td align="center">[8192, 8192]</td><td align="center">float16</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel function name</td><td colspan="4" align="center">quant_group_matmul_custom</td></tr>
</table>

- call implementation
  Use the kernel caller <<<>>> to call the kernel function.

## Sample implementation

### Scenario Description

This example shows the complete optimization path in the Vector bound scenario through three progressive scenarios:

| Scenario | SCENARIO_NUM | __mix__ mode | PIPELINE_DEPTH | Description |
|------|-------------|------------|----------------|------|
| Scenario 0 | 0 | `__mix__(1, 1)` | 1 | Benchmark: Single AIV, no pipeline overlap, Cube and Vector serial |
| Scenario 1 | 1 | `__mix__(1, 2)` | 1 | Dual AIV, no pipeline overlap, multiple AIVs share Vector tasks |
| Scenario 2 | 2 | `__mix__(1, 2)` | 4 | Dual AIV + 4-level pipeline, Cube-Vector fine-grained pipeline overlap |

**Key code**:
```cpp
constexpr uint32_t scenarioNum = SCENARIO_NUM;
// ...
constexpr uint32_t PIPELINE_DEPTH = (scenarioNum >= 2) ? 4 : 1;

template <...>
#if SCENARIO_NUM == 0
__global__ __mix__(1, 1)
#else
__global__ __mix__(1, 2)
#endif
void quant_group_matmul_custom(...)
```

### Performance indicator description

| Indicator | Description |
|------------------------|----------------------------------------------------------------------|
| Task Duration(μs) | The total time of the entire task execution. The sample execution time is based on this parameter |
| Block Num | Number of cores used (number of blocks) |
| aicore_time(μs) | Average execution time of AI Core |
| aic_mac_time(μs) | cube type instructions (matrix operation instructions) time consuming |
| aic_mac_ratio | The ratio of the cycle number of cube type instructions (matrix operation instructions) to the total cycle number |
| aic_mte1_time(μs) | mte1 type instruction (L1 -> L0A/L0B transfer) time consuming |
| aic_mte1_ratio | The ratio of the cycle number of mte1 type instructions (L1 -> L0A/L0B transfer) to the total cycle number |
| aic_mte2_time(μs) | mte2 type instruction (GM -> L1 transfer) time consuming |
| aic_mte2_ratio | The ratio of the cycle number of mte2 type instructions (GM -> L1 transfer) to the total cycle number |
| aic_fixpipe_time(μs) | Fixpipe type instruction (L0C -> GM/UB transfer) time consuming |
| aic_fixpipe_ratio | The ratio of the cycle number of the fixpipe type instruction (L0C -> GM/UB transfer) to the total cycle number |
| aiv_time(μs) | Task execution time on AI Vector Core |
| aiv_vec_time(μs) | vec type instructions (vector operation instructions) time consuming |
| aiv_vec_ratio | The ratio of the cycle number of vec type instructions (vector type operation instructions) to the total cycle number |
| aiv_scalar_time(μs) | scalar type instructions (scalar operation instructions) time consuming |
| aiv_scalar_ratio | The ratio of the cycle number of scalar type instructions (scalar operation instructions) to the total cycle number |
| aiv_mte2_time(μs) | mte2 type instruction (GM -> UB handling instruction) time consuming |
| aiv_mte2_ratio | The ratio of the cycle number of mte2 type instructions (GM -> UB transfer instructions) to the total cycle number |
| aiv_mte3_time(μs) | mte3 type instructions (UB -> GM handling instructions) time consuming |
| aiv_mte3_ratio | The ratio of the cycle number of mte3 type instructions (UB -> GM handling instructions) to the total cycle number |

### Data flow path

```text
Storage location Data type Pipeline

GM (x, weight)                   int8
  |                                               MTE2
L1                               int8
  |                                               MTE1
L0A / L0B                        int8
  |  Iterate                                     MMAD
L0C                              int32
  |                                               FIXPIPE
GM (workspace)                   int32
  |  DataCopyPad2D                                MTE2
UB vecInQueue                    int32
  |  AscendDequant (×scale)   int32 → float       VECTOR
UB dequantMiddleResult            float
  |  Mul (×perTokenScale)     float → float       VECTOR
UB mulsResultLocal                float
  |  FasterGelu               float → float       VECTOR
UB actResultLocal                 float
  |  Cast                     float → half        VECTOR
UB vecOutQueue                    half
  |  DataCopyPad2D                                MTE3
GM (y)                            half

Note: scale[float] and perTokenScale[float] are moved into UB independently through MTE2.
    Participate in the AscendDequant and Mul steps respectively.
```

### Core Features

#### Case 0: Baseline implementation (`__mix__(1, 1)`, PIPELINE_DEPTH=1)

Case 0 is the basic scenario in this example, using `__mix__(1, 1)` mode (1 AIC + 1 AIV), PIPELINE_DEPTH=1. Cube and Vector are synchronized serially at Iterate granularity - Cube notifies Vector after completing an Iterate, and Vector notifies Cube after processing, so there is no overlap in pipeline. On this basis, Case 0 already includes the following optimization features:

**① Cube+Vector fusion calculation**

This example uses the `__mix__` kernel function mode, and AIC (Cube core) and AIV (Vector core) run in parallel on the same physical core. AIC performs matrix multiplication, and AIV performs inverse quantization + activation + type conversion. The two are synchronized through `CrossCoreSetFlag`/`CrossCoreWaitFlag`:

```cpp
// AIC execution path: MMCompute → Iterate loop → SetFlag(SYNC_AIC_TO_AIV)
// AIV execution path: Skip MMCompute → VectorCompute → WaitFlag(SYNC_AIC_TO_AIV)
if ASCEND_IS_AIC {
    matmulObj.Iterate();    // Cube calculation
    CrossCoreSetFlag<2, PIPE_FIX>(SYNC_AIC_TO_AIV);  // NotificationVector
}
if ASCEND_IS_AIV {
    CrossCoreWaitFlag<2>(SYNC_AIC_TO_AIV);  // Wait for Cube to complete
    // Process data...
    CrossCoreSetFlag<2, PIPE_MTE2>(SYNC_AIV_TO_AIC);
}
```

In Case 0, PIPELINE_DEPTH=1, the workspace has only 1 Slot (that is, a continuous space of size `CORE_NUM×BASE_M×BASE_N×sizeof(int32_t)`), and Cube must wait for Vector to be processed after each Iterate before reusing the Slot. Cube and Vector are executed serially.

**② Continuous polling across Groups (preCount mechanism)**

The Process function uses the **continuous polling across Group** strategy to allocate sub-blocks to multiple Cores. The core idea is to logically number the sub-blocks of all Groups consecutively. Each Core polls and retrieves blocks according to the `CORE_NUM` step size, and uses the `preCount` variable to record the remainder at the end of the previous Group that is not divisible by `CORE_NUM` to achieve continuous distribution across Groups and avoid uneven load between cores.

**Blocking dimension description**:

- **GroupM**: The number of M-dimensional rows of the current Group, dynamically read from groupList. In this example, each group has GroupM=1024
- **numBlocksM = Ceil(GroupM, SINGLE_M)**: The number of sub-blocks of the current Group in the M direction. Ceil(1024, 128) = 8 under Atlas A2, Ceil(1024, 256) = 4 under Ascend 950PR
- **numBlocksN = Ceil(N, SINGLE_N)**: The number of sub-blocks in the N direction, the same for all Groups. Ceil(8192, 1024) = 8

Each Group has a total of `numBlocksM × numBlocksN` sub-blocks, which are positioned by `(mIdx, nIdx)` two-dimensional coordinates. The size of each sub-block is `[SINGLE_M, SINGLE_N]`, and the Cube internal calculation is based on `[BASE_M, BASE_N]` granularity (each sub-block requires `SINGLE_N/BASE_N × SINGLE_M/BASE_M` Iterate times).

```cpp
for (groupIdx = 0, preCount = 0; groupIdx < GROUP_NUM; ++groupIdx) {
    groupM = groupListGlobal.GetValue(groupIdx);          // The number of rows in this group, in this example =1024
    numBlocksM = Ceil(groupM, SINGLE_M);                   // Number of blocks in M ​​direction
    numBlocksN = Ceil(N, SINGLE_N);                         // Number of blocks in N direction, same for all groups = 8
    curCount = preCount + numBlocksM * numBlocksN;          // The upper bound of consecutive numbers including the remainder of the previous group

    // Core allocation formula: determine the first block number of the current Core in this Group
    curBlock = coreIdx >= preCount ? coreIdx : coreIdx + CORE_NUM;

    while (curBlock < curCount) {
        mIdx = (curBlock - preCount) / numBlocksN;          // M direction block index within group
        nIdx = (curBlock - preCount) % numBlocksN;          // N-direction block index within the group
        MMCompute(groupIdx, groupConfig);                    // Cube calculation
        VectorCompute(groupIdx, groupConfig);                // Vector calculation
        curBlock += CORE_NUM;                                // Polling step, the step size is CORE_NUM
    }
    preCount = curCount % CORE_NUM;                          // The remainder is passed to the next group
    globalRowOffset += groupM;                               // Global row offset accumulation
}
```

**preCount cross-group continuous allocation mechanism**: The initial value of `curBlock` is determined by `preCount` - if `coreIdx >= preCount`, take the block directly from `coreIdx`, otherwise skip the first `preCount` blocks of this group and start from `coreIdx + CORE_NUM`. This ensures that cores allocated more at the end of the upper group are less allocated at the beginning of the lower group.

**Atlas A2 sample example** (CORE_NUM=24, 64 blocks per group, 64 % 24 = 16):

```
Group 0: preCount=0, curCount=64 → Core0~15 processes 3 blocks, Core16~23 processes 2 blocks → preCount_out=16
Group 1: preCount=16, curCount=80 → Core0~7 processes 3 blocks, Core8~15 processes 2 blocks, Core16~23 processes 3 blocks → preCount_out=8
Group 2: preCount=8, curCount=72 → Core0~7 processes 3 blocks, Core8~23 processes 2 blocks → preCount_out=0
Group 3~7: preCount cycles through 3 groups 0 → 16 → 8 → 0 → ...
```

There are 512 blocks in total in 8 groups. Core0~7 each process 22 blocks, and Core8~23 each process 21 blocks. The maximum load difference is only 1 block.

**③ Tiling constantization**

All Tiling parameters are determined at compile time through `MatmulApiStaticTiling`. No Scalar dynamic calculation is required at runtime, reducing Scalar overhead:

```cpp
constexpr static auto CONSTANT_CFG = GetCustomConstantCFG();
matmul::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CONSTANT_CFG> matmulObj;
```


**④ Matmul UnitFlag optimization**

This example turns on Matmul's UnitFlag optimization through `mmCFG.enUnitFlag = true` to achieve 512B fine-grained pipeline synchronization of Cube calculation and handling:

```cpp
static constexpr MatmulShapeParams shapeParams = {SINGLE_M, SINGLE_N, K, BASE_M, BASE_N, BASE_K};
MatmulConfig mmCFG = GetMMConfig<MatmulConfigMode::CONFIG_MDL>(shapeParams);
mmCFG.enUnitFlag = true;  // Turn on UnitFlag
auto constantCFG = AscendC::GetMatmulApiTiling<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>(mmCFG);
```

By default, Cube's MMAD calculation and FixPipe transfer are coarse-grained synchronization - you must wait for the calculation of the entire `[BASE_M, BASE_N]` block to be completed before FixPipe can start moving out data. After UnitFlag is turned on, MMAD and FixPipe implement fine-grained (512B) pipeline parallelism: whenever Cube completes the calculation of a 512B data result, FixPipe immediately moves out the data, and the Cube calculation overlaps with the result write-back pipeline.

**⑤ UB Double Buffer**

In this example, all TQue queues are set to `BUFFER_NUM=2` to enable double buffering to achieve parallel data transfer and calculation on the Vector side:

```cpp
constexpr uint32_t BUFFER_NUM = 2; // Number of UB queue buffers, set to 2 to enable double buffering

pipe->InitBuffer(scaleInQueue, BUFFER_NUM, BASE_N * sizeof(float));          // 2 × 1024B
pipe->InitBuffer(perTokenScaleInQueue, BUFFER_NUM, BASE_M * sizeof(float));   // DAV_2201(BASE_M=128): 2 × 512B；DAV_3510(BASE_M=256): 2 × 1024B
pipe->InitBuffer(vecInQueue, BUFFER_NUM, UB_CAL_SIZE * sizeof(int32_t));      // 2 × 24,576B
pipe->InitBuffer(vecOutQueue, BUFFER_NUM, UB_CAL_SIZE * sizeof(half));        // 2 × 12,288B
```

The working principle of double buffer is: when Vector is processing the data of Buffer A, MTE2/MTE3 can handle the data of Buffer B at the same time, and vice versa.

```
Time──────────────────────────────────────────────────────>
MTE2: |─ Move into Buffer0 ──|─ Move into Buffer1 ──|─ Move into Buffer0 ──| ...
VEC: |─ Process Buffer0 ──|─ Process Buffer1 ──|─ Process Buffer0 ──| ...
MTE3: |─ Move out Buffer0 ──|─ Move out Buffer1 ─| ...
```

**Performance Data**

Take the measured performance of Ascend 950PR as an example:

| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 0 | MIX_AIC | 282.717 | 32 | 32 | 281.84 | 167.952 | 0.596 | 34.137 | 0.121 | 50.792 | 0.18 | 148.151 | 0.526 | 59.619 | 0.212 | 281.84 | 229.166 | 0.813 | 77.027 | 0.273 | 129.698 | 0.46 | 87.937 | 0.312 |

**Optimization effect analysis**:
- End-to-end time consumption: **282.717μs**
- Cube calculation time: 167.952μs, accounting for **59.6%**
- Vec calculation time: 229.166μs, accounting for **81.3%**

Case 0 is the benchmark scenario. Vector calculation time (229.166μs) significantly exceeds Cube calculation (167.952μs). Vector takes a long time and needs to be optimized.

**Next step optimization direction**: Improve the calculation parallelism on the Vector side, upgrading from `__mix__(1, 1)` to `__mix__(1, 2)` dual AIV mode.

#### Case 1: Dual AIV parallel (`__mix__(1, 2)`, PIPELINE_DEPTH=1)

Case 1 upgrades the kernel function mode from `__mix__(1, 1)` to `__mix__(1, 2)` on the basis of Case 0. Each physical core contains 1 AIC and 2 AIVs. The Vector side computing tasks are shared through multiple AIVs to improve Vector throughput. PIPELINE_DEPTH is still 1, and Cube and Vector are still serially synchronized.

**`__mix__` Dual AIV Mode**

In `__mix__(1, 2)` mode, each physical core contains 1 AIC and 2 AIV. The two AIVs share the offsetM iteration task on the Vector side through the `taskRation` polling mechanism:

```cpp
uint32_t taskRation = AscendC::GetTaskRation();  // __mix__(1,2) is 2
if (taskRation != 0 && vecCount % taskRation != subBlockIdx) {
    continue;  // Skip tasks that are not part of the current AIV
}
```

The two AIVs share the same coreIdx and are identified by `subBlockIdx` (obtained by `GetSubBlockIdx()`). In the inner loop of offsetM, `vecCount` is incremented, and each AIV only processes the iterations of `vecCount % taskRation == subBlockIdx`, that is, even iterations are processed by AIV0, and odd iterations are processed by AIV1, achieving load balancing.

Compared with Case 0's `__mix__(1, 1)` single AIV mode, dual AIV can reduce the Vector calculation time by half, effectively alleviating the calculation bottleneck on the Vector side.

**Performance Data**:
| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 1 | MIX_AIC | 224.125 | 32 | 64 | 223.27 | 168.201 | 0.753 | 33.531 | 0.15 | 50.367 | 0.226 | 153.263 | 0.686 | 104.334 | 0.467 | 223.29 | 112.441 | 0.504 | 39.701 | 0.178 | 77.206 | 0.346 | 43.409 | 0.194 |


**Optimization effect analysis**:
- End-to-end time consumption: **224.125μs**, time consumption reduced by 20.72%
- Cube calculation time: 168.201μs. This optimization is to optimize Vector calculation parallelism. The calculation time on Cube side has not been significantly improved.
- Vec calculation time: 112.441μs, time consumption reduced by 50.93%, in line with expected benefits

From the time-consuming relationship, the end-to-end time consumption is less than the sum of the Cube calculation time and the Vector calculation time (168.201μs + 112.441μs), indicating that there is already partial parallelism between Cube and Vector; however, the end-to-end time consumption is still significantly higher than the larger value of 168.201μs, indicating that the parallel overlap is insufficient.

**PIPE flow chart display**:

As can be seen from the PIPE pipeline diagram, although Case 1 has introduced dual AIVs to share Vector tasks, due to `PIPELINE_DEPTH=1`, Cube and Vector are still serially synchronized according to Iterate granularity, and the workspace only has a single Slot for reuse, so obvious waits and breakpoints can still be seen in the pipeline.

![PIPE flow chart of Case 1 ](./figures/CV流水优化1.png)

**Next step optimization direction**: Introduce 4-level Cube-Vector pipeline to improve Cube-Vector parallelism and further improve overall computing efficiency.

#### Case 2: Level 4 Cube-Vector pipeline (`__mix__(1, 2)`, PIPELINE_DEPTH=4)

Case 2 increases PIPELINE_DEPTH from 1 to 4 based on Case 1, introduces the workspace circular slot reuse mechanism, and realizes Cube-Vector fine-grained pipeline overlap.

**Workspace circular Slot reuse**

The workspace is the transfer station between Cube and Vector, using annular slot design. The Slot here refers to a continuous space located by `workSpaceOffset = BASE_N × BASE_M × (coreIdx + (cubeTaskIdx % PIPELINE_DEPTH) × CORE_NUM)`. The Slot size of a single core is `BASE_M×BASE_N×sizeof(int32_t)`. The total Slot space of each layer of pipeline is `CORE_NUM×BASE_M×BASE_N×sizeof(int32_t)`. `PIPELINE_DEPTH` Slots are used in turn. While the Cube writes the current Slot, the Vector can process the data of the previous Slot:

```
workspace layout:
Slot 0: Core0[BASE_M×BASE_N] + Core1[BASE_M×BASE_N] + ... + Core(CORE_NUM-1)[BASE_M×BASE_N]
Slot 1: Core0[BASE_M×BASE_N] + Core1[BASE_M×BASE_N] + ... + Core(CORE_NUM-1)[BASE_M×BASE_N]
Slot 2: ...
Slot 3: ...

workSpaceOffset = BASE_N × BASE_M × (coreIdx + (cubeTaskIdx % PIPELINE_DEPTH) × CORE_NUM)
```

- When `cubeTaskIdx < PIPELINE_DEPTH`, Cube does not need to wait and directly writes the new Slot.
- When `cubeTaskIdx >= PIPELINE_DEPTH`, Cube needs to wait for Vector to release the corresponding Slot (`CrossCoreWaitFlag<2>(SYNC_AIV_TO_AIC)`)
- The synchronization granularity is Iterate (=BASE_M×BASE_N), not sub-block (=SINGLE_M×SINGLE_N)

**Level 4 pipeline timing**:

```
Time───────────────────────────────────────────────────────────>
Cube:  |─Iter0→Slot0─|─Iter1→Slot1─|─Iter2→Slot2─|─Iter3→Slot3─|─Iter4→Slot0─| ...
                      (Wait)                                     (Wait)
Vec: |─Slot0 processing─|─Slot1 processing─|─Slot2 processing─|─Slot3 processing─| ...
```

The Cube does not need to wait for the first 4 Iterates (Slot 0~3) and writes continuously; when the 5th Iterate reuses Slot 0, it needs to wait for the Vector to be released. This forms a Cube-Vector pipeline overlap, which significantly reduces the Cube idle waiting time compared to the serial mode of Case 1.

**Performance Data**:
| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 2 | MIX_AIC | 204.129 | 32 | 64 | 203.26 | 180.347 | 0.887 | 36.598 | 0.18 | 51.726 | 0.254 | 172.252 | 0.847 | 185.875 | 0.914 | 203.29 | 112.218 | 0.552 | 39.816 | 0.196 | 79.51 | 0.391 | 50.123 | 0.247 |

**Optimization effect analysis**:
- End-to-end time consumption: **204.129μs**, which is reduced by 8.92% compared to Case 1 and **27.80%** compared to Case 0.
- Cube calculation time: 180.347μs, slightly increased compared to Case 1, but `aic_mac_ratio` increased from 0.753 to 0.887, indicating that Cube calculation continuity is better.
- The Vec calculation time is 112.218μs, which is basically the same as the 112.441μs of Case 1. This shows that this round of optimization does not change the single calculation overhead on the Vector side. The main benefit comes from the pipeline overlap between Cube and Vector.

**PIPE flow chart display**:
Case 2 increases `PIPELINE_DEPTH` to 4 based on Case 1. Cube can continuously write different Slots on multiple Iterates, and Cube calculation continuity is better.

![PIPE flow chart of Case 2 ](./figures/CV流水优化2.png)

## Performance data analysis

### Atlas A2 training series chip performance data

| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|
| Scenario 0 | MIX_AIC | 583.512 | 24 | 24 | 564.06 | 192.02 | 0.34 | 115.317 | 0.204 | 168.645 | 0.299 | 141.648 | 0.251 | 79.47 | 0.141 | 565.12 | 471.71 | 0.835 | 228.275 | 0.404 | 139.25 | 0.246 | 94.682 | 0.168 |
| Scenario 1 | MIX_AIC | 353.887 | 24 | 48 | 344.01 | 192.612 | 0.56 | 143.518 | 0.417 | 170.031 | 0.494 | 160.396 | 0.466 | 107.662 | 0.313 | 345.22 | 235.871 | 0.683 | 122.424 | 0.355 | 107.715 | 0.312 | 56.494 | 0.164 |
| Scenario 2 | MIX_AIC | 299.286 | 24 | 48 | 288.65 | 193.576 | 0.671 | 196.668 | 0.681 | 173.003 | 0.599 | 170.193 | 0.59 | 167.936 | 0.582 | 285.21 | 235.872 | 0.827 | 126.227 | 0.443 | 99.321 | 0.348 | 56.645 | 0.199 |


### Ascend 950PR chip performance data
| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 0 | MIX_AIC | 282.717 | 32 | 32 | 281.84 | 167.952 | 0.596 | 34.137 | 0.121 | 50.792 | 0.18 | 148.151 | 0.526 | 59.619 | 0.212 | 281.84 | 229.166 | 0.813 | 77.027 | 0.273 | 129.698 | 0.46 | 87.937 | 0.312 |
| Scenario 1 | MIX_AIC | 224.125 | 32 | 64 | 223.27 | 168.201 | 0.753 | 33.531 | 0.15 | 50.367 | 0.226 | 153.263 | 0.686 | 104.334 | 0.467 | 223.29 | 112.441 | 0.504 | 39.701 | 0.178 | 77.206 | 0.346 | 43.409 | 0.194 |
| Scenario 2 | MIX_AIC | 204.129 | 32 | 64 | 203.26 | 180.347 | 0.887 | 36.598 | 0.18 | 51.726 | 0.254 | 172.252 | 0.847 | 185.875 | 0.914 | 203.29 | 112.218 | 0.552 | 39.816 | 0.196 | 79.51 | 0.391 | 50.123 | 0.247 |


### Performance analysis summary

In this example, the fixed shape is `M=8192, N=8192, K=1024`, and the grouping only changes the M-axis scheduling method. The total Cube calculation amount is still:

$$
Ops_{\text{cube}} = \sum_{i=0}^{G-1} group_i \times N \times K = M \times N \times K
$$

For int8 matrix multiplication, the Cube single cycle is estimated as `16 × 32 × 16` times of multiplication and addition. The theoretical Cube calculation time is:

$$
T_{\text{cube}} =
\frac{M \times N \times K}{16 \times 32 \times 16 \times f \times CORE\_NUM}
$$

The final optimization scenario is Scenario 2 (`__mix__(1, 2)` + 4-level Cube-Vector pipeline). In this scenario, the Cube side utilization and bottleneck are judged as follows:

| Chip | CORE_NUM | Theoretical Cube time consumption (μs) | aic_mac_time(μs) | aic_mac_ratio | aiv_vec_time(μs) | Reaching the theoretical peak ratio | Bottleneck judgment |
|------|----------|------------------|------------------|---------------|------------------|----------------|----------|
| Atlas A2/A3 (DAV_2201) | 24 | 188.93 | 193.576 | 0.671 | 235.872 | 97.60% | Still on the Vector side |
| Ascend 950PR (DAV_3510) | 32 | 158.88 | 180.347 | 0.887 | 112.218 | 88.10% | Not the Vector side anymore |


Judging from the final results, the vector side calculation time on Ascend 950PR has been reduced to 112.218μs through dual AIV parallelization, which is significantly lower than the Cube calculation time of 180.347μs, and `aic_mac_ratio` reached 0.887, indicating that the performance bottleneck is no longer the Vector calculation itself. At this time, the end-to-end time consumption is mainly affected by the overhead of Cube calculation, workspace reuse synchronization, FIXPIPE handling and pipeline closing.

The performance on Atlas A2/A3 is different: `aiv_vec_time` in Scenario 2 is 235.872μs, which is higher than `aic_mac_time`’s 193.576μs, and `aiv_vec_ratio` also reaches 0.827, indicating that the inverse quantization, per-token scaling, FasterGelu and Cast links on the Vector side still occupy a large proportion, and the overall bottleneck is still on the Vector side. At the same time, A2's measured Cube calculation time of 193.576 μs is close to the theoretical Cube time of 188.93 μs, reaching about 97.60% of the theoretical peak, indicating that the Cube matrix multiplication itself has been fully utilized. To continue optimizing end-to-end performance, priority must be paid to the Vector side calculation link and Cube-Vector synchronization overhead.

Although Atlas A2/A3 is still biased towards Vector bound in the end, the three-level optimization is still effective: Scenario 1 reduces the Vector calculation time from 471.710μs to 235.871μs through dual AIV parallelization, and the end-to-end time consumption is reduced from 583.512μs to 353.887μs; Scenario 1 2 While the Vector calculation time is basically unchanged, the overlap between Cube and Vector is improved through 4-level workspace pipeline, and the end-to-end time is further reduced to 299.286μs, which is a 48.71% reduction in total time compared to Scenario 0.


## Compile and run

Perform the following steps in the root directory of this sample to compile and execute the sample.
- Configure environment variables
  Please configure the environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If the installation directory is not specified, it will be installed under `/usr/local/Ascend` by default.

- Sample execution

  Execute the following command in this sample directory.
  ```bash
  SCENARIO_NUM=1                                                       # Select execution scenario
  mkdir -p build && cd build;                                          # Create and enter the build directory
  cmake -DCMAKE_ASC_RUN_MODE=npu -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # Compile project
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output results are correct and confirm that the algorithm logic is correct
  ```

  When using NPU simulation mode, just add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples are as follows:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU emulation mode
  ```

  > **Note:** You need to clear the cmake cache before switching the compilation mode. You can execute `rm CMakeCache.txt` in the build directory and then re-cmake.

- Description of compilation options

  | options | optional values ​​| description |
  |------|--------|------|
  | `SCENARIO_NUM` | `0`, `1`, `2` | Scenario number: 0-baseline single AIV, 1-double AIV with no flow, 2-double AIV+4-level flow |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Operation mode: NPU operation, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 training series products/Atlas A2 inference series products and Atlas A3 training series products/Atlas A3 inference series products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution results
  The execution results are as follows, indicating that the accuracy comparison is successful.
  ```bash
  test pass!
  ```

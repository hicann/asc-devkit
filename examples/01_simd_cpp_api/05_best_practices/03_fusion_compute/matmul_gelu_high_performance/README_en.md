# Matmul Gelu Best Practice Example

## Overview

This example demonstrates Cube-Vector (CV) fusion implementation based on the static Tensor programming paradigm, fusing Matmul matrix multiplication with GELU activation function to execute in parallel within the same AI Core. The AIC side completes Matmul computation and outputs results to GM or UB through Fixpipe, while the AIV side reads data from GM or UB to complete GELU vector computation. The Matmul optimization details refer to the [Matmul Basic API Best Practice Example](../../01_matrix_compute/matmul_basic_api_high_performance/README_en.md), and the GELU optimization details refer to the [Gelu Performance Tuning Example](../../02_reg_compute/gelu_high_performance/README_en.md).

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── matmul_gelu_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Ground truth comparison script
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── figures                 // Illustrations
│   ├── mmad_gelu.asc           // Ascend C example implementation
│   └── README.md               // Example documentation
```

## Example Description

Matmul + GELU computation formula:

$$
C = GELU(A * B)
$$

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot (x + 0.044715 \cdot x^3)}}
$$

- A is the left matrix, shape [M, K], data type float16; B is the right matrix, shape [K, N], data type float16, B matrix stored in transposed format
- C is the destination operand, storing Matmul+GELU computation result, shape [M, N], data type float32

- Example specifications:

<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">Matmul + GELU</td></tr>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
<tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">float16</td><td align="center">ND</td><td align="center">false</td></tr>
<tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">float16</td><td align="center">ND</td><td align="center">true</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">float32</td><td align="center">ND</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">mmad_vector_custom</td></tr>
</table>

## Example Implementation

### Performance Metric Description

| Metric                    | Description                                                                 |
|------------------------|----------------------------------------------------------------------|
| Task Duration(μs)      | Total execution time of the entire task, operator execution time is determined by this parameter                         |
| Block Num              | Number of cores used (Block count)                                              |
| aicore_time(μs)        | Average AI Core execution time                                                |
| aic_mac_time(μs)       | Cube type instruction (matrix computation instructions) duration                                    |
| aic_mac_ratio          | Ratio of cube type instruction cycle count to total cycle count    |
| aic_mte1_time(μs)      | MTE1 type instruction (L1 -> L0A/L0B transfer) duration                               |
| aic_mte1_ratio         | Ratio of MTE1 type instruction cycle count to total cycle count |
| aic_mte2_time(μs)      | MTE2 type instruction (GM -> L1 transfer) duration                                    |
| aic_mte2_ratio         | Ratio of MTE2 type instruction cycle count to total cycle count    |
| aic_fixpipe_time(μs)   | Fixpipe type instruction (L0C -> GM/UB transfer) duration                             |
| aic_fixpipe_ratio      | Ratio of fixpipe type instruction cycle count to total cycle count |
| aiv_time(μs)           | Task execution time on AI Vector Core                               |
| aiv_vec_time(μs)       | vec type instruction (vector computation instructions) duration                                    |
| aiv_vec_ratio          | Ratio of vec type instruction cycle count to total cycle count    |
| aiv_scalar_time(μs)    | Scalar type instruction (scalar computation instructions) duration                                 |
| aiv_scalar_ratio       | Ratio of scalar type instruction cycle count to total cycle count |
| aiv_mte2_time(μs)      | MTE2 type instruction (GM -> UB transfer instructions) duration                              |
| aiv_mte2_ratio         | Ratio of MTE2 type instruction cycle count to total cycle count |
| aiv_mte3_time(μs)      | MTE3 type instruction (UB -> GM transfer instructions) duration                              |
| aiv_mte3_ratio         | Ratio of MTE3 type instruction cycle count to total cycle count |

### Data Flow Path

This example provides two CV fusion scenarios:

**Scenario 1: CV Fusion-GM Relay (Intermediate Performance Optimization)** (Supported by A2/A3/Ascend 950PR)

```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> GM ──(MTE2)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

**Scenario 2: CV Fusion-UB Direct (Advanced Extreme Refinement)** (Only Ascend 950PR supported)

Ascend 950PR adds the L0C->UB path, which A2/A3 does not support. With enhanced chip capabilities, in the Ascend 950PR architecture, matrix multiplication results can be output directly to UB for GELU computation without going through GM.

```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

### Intermediate Performance Optimization

The Matmul part of this example uses the same optimization methods as the [Matmul Basic API Best Practice Example](../../01_matrix_compute/matmul_basic_api_high_performance/README_en.md) (L1/L0 double buffering, large packet transfer, fine-grained pipeline synchronization, UnitFlag, etc.). The GELU part uses RegBase method on Ascend 950PR architecture and MemBase method on A2/A3 architecture. For detailed GELU formula simplification, RegBase API and VF fusion optimization analysis, please refer to the [Gelu Performance Tuning Example](../../02_reg_compute/gelu_high_performance/README_en.md).

#### Matmul Optimization Methods

| Case | Optimization Method | Description |
|------|---------|------|
| [Case 1](../../01_matrix_compute/matmul_high_performance/README_en.md#case-1-single-core-tiling-optimization-single_core_tiling) | Single-core Tiling optimization | Optimize base block parameters, reduce memory-to-computation ratio |
| [Case 2](../../01_matrix_compute/matmul_high_performance/README_en.md#case-2-multi-core-split-2x12-multi_core_split_2_12) | Multi-core splitting 2x12 | Introduce multi-core parallel computation, balanced load distribution |
| [Case 3](../../01_matrix_compute/matmul_high_performance/README_en.md#case-3-multi-core-split-4x6-multi_core_split_4_6) | Multi-core splitting 4x6 | Optimize multi-core splitting strategy, 512B address alignment, reduce same-address access |
| [Case 4](../../01_matrix_compute/matmul_high_performance/README_en.md#case-4-multi-core-using-mdl-template-multi_core_mdl) | Multi-core using MDL template | Enable L1 multi-block cache, large packet transfer, reduce MTE2 loop transfer count |

#### Gelu Optimization Methods

| Case | Optimization Method | Description |
|------|---------|------|
| [Case 1](../../02_reg_compute/gelu_high_performance/README_en.md#case-1-enable-regbase-api-and-vf-fusion) | RegBase API + VF fusion | Register-level computation reduces intermediate Load/Store, utilizes dual-issue feature to improve IPC |

#### GELU Computation Implementation

**A2/A3 Architecture**: Uses MemBase method (vector basic API + PipeBarrier), no additional intermediate buffers needed, directly reuses output buffer:

```cpp
__aicore__ inline void GeluMemBaseCompute(
    const AscendC::LocalTensor<float>& xLocal, const AscendC::LocalTensor<float>& yLocal, uint32_t n)
{
    AscendC::Mul(yLocal, xLocal, xLocal, n);        // x²
    AscendC::PipeBarrier<PIPE_V>();
    AscendC::Mul(yLocal, yLocal, xLocal, n);        // x³
    ……
}
```

**Ascend 950PR Architecture**: Uses RegBase method (VF function), intermediate computation completed within registers, no additional UB buffers needed, reducing data exchange between UB and Reg:

```cpp
__simd_vf__ inline void GeluVf(__ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;

    for (uint16_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(n);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);       // x²
        ……
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);
    }
}
```

The following focuses on the AIC↔AIV CV fusion mechanism.

#### 1. CV Fusion Architecture: `__mix__(1,2)` Kernel Function

This example uses `__global__ __mix__(1, 2)` to declare the kernel function, meaning each logical core consists of 1 AIC and 2 AIVs. The AIC side is responsible for Matmul computation, the AIV side is responsible for GELU vector computation, and both execute in pipeline parallelism within the same AI Core at (baseM × baseN) block granularity.

```cpp
__global__ __mix__(1, 2) void mmad_vector_custom(__gm__ uint8_t* xMatrix, __gm__ uint8_t* yMatrix, __gm__ uint8_t* zMatrix)
{
    AscendC::InitSocState();
    KernelMmadVector<...> op;
    op.Init(xMatrix, yMatrix, zMatrix);
    op.Process();
    AscendC::PipeBarrier<PIPE_ALL>();
}
```

In `Process()`, `ASCEND_IS_AIC` and `ASCEND_IS_AIV` macros distinguish AIC and AIV execution paths:

```cpp
if ASCEND_IS_AIC {
    // AIC side: Buffer allocation + computation loop + Fixpipe transfer out
    ProcessLoopAic(...);
}
if ASCEND_IS_AIV {
    // AIV side: GELU vector computation + transfer out to GM
    ProcessLoopAiv();
}
```

#### 2. Cross-Core Synchronization: AIC→AIV Data Readiness Notification

After the AIC side completes Fixpipe output of one (baseM × baseN) sub-block, it notifies the AIV side that data is ready through `CrossCoreSetFlag`; the AIV side waits for this signal through `CrossCoreWaitFlag`, then starts GELU computation:

```cpp
// AIC side CopyOutAic():
AscendC::CrossCoreSetFlag<0x2, PIPE_FIX>(0x8);  // Notify AIV data ready

// AIV side GeluAndCopyOutAivSeparate() / GeluAndCopyOutAiv():
AscendC::CrossCoreWaitFlag(0x8);  // Wait for AIC side Fixpipe completion
```

#### 3. Multi-Core Splitting in `__mix__(1,2)` Mode

In `__mix__(1,2)` mode, `GetBlockIdx()` is independently numbered on the AIC side and AIV side: AIC side returns 0~numAICores-1, AIV side returns 0~numAIVCores-1. Since each logical core corresponds to 2 AIVs, the AIV side needs to map `GetBlockIdx() / 2` back to the logical core ID, thereby using the same multi-core splitting strategy as the AIC side:

```cpp
uint32_t logicCoreId;
if ASCEND_IS_AIC {
    logicCoreId = AscendC::GetBlockIdx();
} else {
    logicCoreId = AscendC::GetBlockIdx() / 2;  // __mix__(1,2): Each logical core corresponds to 2 AIVs
}
uint32_t mIterIdx = logicCoreId % mIter;
uint32_t nIterIdx = logicCoreId / mIter;
```

### Advanced Extreme Refinement

In the advanced extreme refinement stage, the CV fusion level provides two data paths: Scenario 1 achieves CV fusion through GM relay (supported by all architectures), Scenario 2 achieves CV fusion through Fixpipe direct to UB (only Ascend 950PR supported), eliminating GM relay read/write overhead.
For Matmul and Gelu related advanced extreme refinement methods, please refer to the list below.

#### Matmul Optimization Methods

| Case | Optimization Method | Description |
|------|---------|------|
| [Case 5](../../01_matrix_compute/matmul_high_performance/README_en.md#case-5-multi-core-mdl--l1cache-optimization-multi_core_mdl_l1cache) | MDL + L1Cache optimization | Manually adjust Tiling parameters, fully utilize L1 cache |
| [Case 6](../../01_matrix_compute/matmul_high_performance/README_en.md#case-6-multi-core-mdl--l1cache--l2cache-multi_core_mdl_l1cache_l2cache) | MDL + L1Cache + L2Cache | Block computation adapted to L2Cache, improve cache hit rate |
| [Case 7](../../01_matrix_compute/matmul_high_performance/README_en.md#case-7-multi-core-mdl--l1cache--l2cache--constants-tiling-multi_core_mdl_l1cache_l2cache_constants) | + Constants Tiling | Tiling constantification, reduce runtime Scalar computation overhead |
| [Case 8](../../01_matrix_compute/matmul_high_performance/README_en.md#case-8-multi-core-mdl--l1cache--l2cache--constants-tiling--unitflag-multi_core_mdl_l1cache_l2cache_constants_unitflag) | + UnitFlag | Fine-grained pipeline synchronization, MMAD and FIXPIPE pipeline parallelism |

#### Gelu Optimization Methods

| Case | Optimization Method | Description |
|------|---------|------|
| [Case 2](../../02_reg_compute/gelu_high_performance/README_en.md#case-2-enable-regbase-api-vf-fusion-and-loop-unrolling-optimization) | VF fusion + loop unrolling | #pragma unroll loop unrolling, improve instruction-level parallelism |

#### CV Fusion-GM Relay

**Scenario 1: CV Fusion-GM Relay** (Supported by A2/A3/Ascend 950PR)

The AIC side writes L0C computation results to GM through Fixpipe, the AIV side reads data from GM to UB for GELU computation, then writes results back to GM. This method is supported by all architectures.

Data flow path:
```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> GM ──(MTE2)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

**AIC Side Implementation**

Fixpipe L0C → GM

```cpp
// A2/A3 architecture
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.nSize = curN;
fixpipeParams.mSize = curM;
fixpipeParams.srcStride = curMAlign;
fixpipeParams.dstStride = N;
AscendC::Fixpipe<float, float, CFG_ROW_MAJOR_GM>(cGM[...], cLocal, fixpipeParams);

// Ascend 950PR architecture
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
fixpipeParams.mSize = curM;
fixpipeParams.nSize = curN;
fixpipeParams.srcStride = curMAlign;
fixpipeParams.dstStride = N;
AscendC::Fixpipe<float, float, CFG_ROW_MAJOR_GM>(cGM[...], cLocal, fixpipeParams);
```

**AIV Side Implementation**

GM → xUB → GELU → geluOutUB → GM

In `__mix__(1,2)` mode, each logical core has 2 AIVs. The M rows of data output by Fixpipe to GM are split into M/2 rows for the two AIVs to process. Each AIV obtains its sub-block index through `GetSubBlockIdx() % 2` and independently completes GELU computation for its respective M/2 rows:

```cpp
uint32_t localSubIdx = AscendC::GetSubBlockIdx() % 2;
uint32_t gmOffset = mBlockIdx * baseM * N + nBlockIdx * baseN + localSubIdx * baseM / 2 * N;

// Step 1: DataCopy GM → xUB
AscendC::DataCopyPad<float>(xUB, cGM[gmOffset], copyInParams, padParams);

// Step 2: GELU computation
// A2/A3: GeluMemBaseCompute(xUB, geluOutUB, computeLen);
// Ascend 950PR:    GeluRegBaseCompute(xUB, geluOutUB, computeLen);

// Step 3: DataCopy geluOutUB → GM
AscendC::DataCopyPad<float>(cGM[gmOffset], geluOutUB, copyOutParams);
```

**Performance Data**

> The following data is based on msOpProf tool measurement, taking the median of 5 runs. Matrix specifications M=8192, K=8192, N=8192, input data type float16, output data type float32.

##### Atlas A2 Training Series Chip

- singleCoreM=2048, singleCoreN=1536, 24 cores

| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|
| Gelu Computation | AI_VECTOR_CORE | 369.827 | 48 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 358.91 | 236.506 | 0.659 | 8.293 | 0.023 | 316.672 | 0.882 | 150.033 | 0.418 |
| Matmul Computation | AI_CORE | 4227.605 | 24 | 0 | 3760.55 | 3083.197 | 0.82 | 338.636 | 0.09 | 2516.081 | 0.669 | 3685.713 | 0.98 | 193.704 | 0.052 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| Scenario 1 | MIX_AIC | 4236.944 | 24 | 48 | 3770.16 | 3083.235 | 0.818 | 531.971 | 0.141 | 2520.852 | 0.669 | 3687.839 | 0.978 | 193.933 | 0.051 | 3773.6 | 174.327 | 0.046 | 2.016 | 0.001 | 90.315 | 0.024 | 66.373 | 0.018 |

##### Ascend 950PR Chip

- singleCoreM=2048, singleCoreN=1024, 32 cores

| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Gelu Computation (RegBase) | AI_VECTOR_CORE | 348.868 | 64 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 347.99 | 66.277 | 0.19 | 3.03 | 0.009 | 320.543 | 0.921 | 314.547 | 0.904 |
| Matmul Computation | AI_CORE | 2601.311 | 32 | 0 | 2600.55 | 2593.264 | 0.997 | 104.582 | 0.04 | 816.91 | 0.314 | 1875.057 | 0.721 | 252.163 | 0.097 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| Scenario 1 | MIX_AIC | 2606.91 | 32 | 64 | 2605.9 | 2597.856 | 0.997 | 105.131 | 0.04 | 818.781 | 0.314 | 1917.737 | 0.736 | 252.231 | 0.097 | 2605.93 | 59.897 | 0.023 | 1.783 | 0.001 | 57.145 | 0.022 | 50.516 | 0.019 |

##### Performance Data Interpretation

Taking Ascend 950PR chip data as example:
- Standalone Gelu task duration 348.868μs, standalone Matmul task duration 2601.311μs
- Scenario 1 fused total duration 2606.91μs, basically consistent with Matmul standalone execution time

This indicates that when using CV fusion, AIV Vector computation time is basically completely covered by AIC Cube computation time. Since Cube and Vector can execute in parallel, the overall task execution time is not a simple C+V overlay, but basically equals Cube computation time (that is, Matmul time), achieving efficient pipeline parallelism.

The figure below shows the pipeline parallel execution process of AIC and AIV in the Scenario 1 (CV Fusion-GM Relay) scenario. The AIC side completes Matmul computation and writes results to GM through Fixpipe, while the AIV side reads data from GM to complete GELU computation, both in pipeline parallelism at (baseM × baseN) block granularity:

![Scenario1 CV Fusion-GM Relay Pipeline Parallelism](figures/CVParallell_L0C_GM_UB.png)

> 💡 The msOpProf tool requires the CANN commercial or community edition. For details, see the [msOpProf Tool Installation Guide](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md). The pipeline diagram was collected on device with `msopprof --aic-metrics=PipeTimeline ./demo`.

**Reason for Significant MTE2 Time Reduction:**

Comparing data reveals:
- Standalone Gelu computation aiv_mte2_time is 320.543μs
- Scenario 1 aiv_mte2_time is only 57.145μs, reduced by approximately 82.2%

Reason analysis: Although AIV also needs to read data from GM to UB in Scenario 1, after AIC's Fixpipe writes Matmul computation results to GM, the data is cached in L2 Cache. When AIV initiates MTE2 operations to read data from GM, since the data is already in L2 Cache, it actually reads from L2 Cache rather than GM, so MTE2 time is significantly reduced. This is also an implicit performance optimization point in CV fusion scenarios.

#### CV Fusion-UB Direct

**Scenario 2: CV Fusion-UB Direct** (Only Ascend 950PR supported)

The AIC side writes L0C computation results directly to UB through Fixpipe. The AIV side does not need to read data from GM, directly performs GELU computation on UB, then writes results back to GM. This method eliminates GM relay read/write overhead, but only Ascend 950PR architecture supports Fixpipe from L0C to UB.

Ascend 950PR adds the L0C->UB path, which A2/A3 does not support. With enhanced chip capabilities, in the Ascend 950PR architecture, matrix multiplication results can be output directly to UB for GELU computation without going through GM.

Data flow path:
```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

**AIC Side Implementation**

Fixpipe L0C → UB (Enable dual destination mode through `dualDstCtl=0b01`, split by M dimension to 2 AIVs)

```cpp
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
fixpipeParams.mSize = DivCeil(curM, 2) * 2;
fixpipeParams.nSize = curN;
fixpipeParams.srcStride = curMAlign;
fixpipeParams.dstStride = curN;
fixpipeParams.dualDstCtl = 0b01;  // Split by M dimension to 2 AIV UBs
AscendC::Fixpipe<float, float, CFG_ROW_MAJOR_UB>(xUB, cLocal, fixpipeParams);
```

**AIV Side Implementation**

UB → GELU → geluOutUB → GM

AIV side does not need DataCopy GM→UB, directly performs GELU computation on xUB:

```cpp
// GELU computation: xUB as input, geluOutUB as output
GeluRegBaseCompute(xUB, geluOutUB, computeLen);

// DataCopy: geluOutUB → GM
uint32_t localSubIdx = AscendC::GetSubBlockIdx() % 2;
uint32_t offset = baseM / 2 * N;
AscendC::DataCopyPad<float>(cGM[mBlockIdx * baseM * N + nBlockIdx * baseN + localSubIdx * offset], geluOutUB, copyParams);
```

**Performance Data**

> The following data is based on msOpProf tool measurement, taking the median of 5 runs. Matrix specifications M=8192, K=8192, N=8192, input data type float16, output data type float32.

##### Ascend 950PR Chip

- singleCoreM=2048, singleCoreN=1024, 32 cores

| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 2 | MIX_AIC | 2573.584 | 32 | 64 | 2572.67 | 2565.251 | 0.997 | 104.669 | 0.041 | 819.934 | 0.319 | 2096.38 | 0.815 | 212.403 | 0.083 | 2572.69 | 60.438 | 0.023 | 1.138 | 0 | 0.005 | 0 | 68.76 | 0.027 |

##### Performance Data Interpretation

Taking Ascend 950PR chip data as example:
- Scenario 2 fused total duration 2573.584μs, further reduced by approximately 33μs compared to Scenario 1's 2606.91μs
- Scenario 2 aiv_mte2_time is only 0.005μs, compared to Scenario 1's 57.145μs, nearly zero

The figure below shows the pipeline parallel execution process of AIC and AIV in the Scenario 2 (CV Fusion-UB Direct) scenario. The AIC side completes Matmul computation and writes results directly to UB through Fixpipe (without GM relay), while the AIV side directly performs GELU computation on UB, eliminating the GM→UB MTE2 transfer overhead:

![Scenario2 CV Fusion-UB Direct Pipeline Parallelism](figures/CVParallell_L0C_UB.png)

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Switch Case

  Specify the scenario to build through `-DSCENARIO_NUM=N` during cmake build. Scenario descriptions:
  - `1`: CV Fusion-GM Relay (Supported by A2/A3/Ascend 950PR)
  - `2`: CV Fusion-UB Direct (Only Ascend 950PR supported)

  > **Notice:** A2/A3 architecture only supports Scenario 1 (GM relay). Selecting Scenario 2 will trigger a compile-time `static_assert` error.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic correctness
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes or Scenarios. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`, `dav-3510` | NPU hardware architecture: dav-2201 corresponds to A2/A3, dav-3510 corresponds to Ascend 950PR |
  | `SCENARIO_NUM` | `1`, `2` | Scenario number: 1=CV Fusion-GM Relay, 2=CV Fusion-UB Direct (only Ascend 950PR supported) |

- Execution results

  The following execution result indicates that the accuracy comparison succeeded.
  ```bash
  test pass!
  ```

## Function Debugging

### printf

This interface provides formatted output functionality in CPU domain/NPU domain debugging scenarios.

Call the printf interface in the operator kernel implementation code where log information needs to be output.

Example:

```cpp
AscendC::printf("matmul blockIdx=%d\n", AscendC::GetBlockIdx());
```

> **Notice:** The printf (PRINTF) interface printing functionality will impact actual operator running performance and is typically used during the debugging phase. Developers can disable printing by setting ASCENDC_DUMP=0 as needed.

### DumpTensor

For operators developed based on operator projects, this interface can be used to dump the content of a specified [LocalTensor](../../../../../docs/zh/api/SIMD-API/basic_api/data_structures/LocalTensor/LocalTensor_intro.md). It also supports printing custom additional information (only uint32\_t data type information is supported), such as printing the current line number.

Call the DumpTensor interface in the operator kernel implementation code where Tensor data needs to be printed. Example:

```cpp
// Vector core LeakyRelu result
AscendC::Div(yLocal, xLocal, yLocal, n);
AscendC::DumpTensor(yLocal, 1, 16);
```

> **Notice:** The [DumpTensor](../../../../../docs/zh/api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor.md) interface printing functionality will impact actual operator running performance and is typically used during the debugging phase. Developers can disable printing by setting ASCENDC_DUMP=0 as needed.

## Performance Debugging

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

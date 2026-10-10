# CrossCoreSetFlag and CrossCoreWaitFlag Inter-Core Synchronization Example

## Overview
This example demonstrates modes 0, 1, 2, and 4 of the [CrossCoreSetFlag](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) inter-core synchronization interfaces in pure Vector and Cube-Vector fused computation scenarios. Mode 4 is supported only on Ascend 950PR/Ascend 950DT.

<p align="center">
  <img src="figures/sync_control_mode_diagram.png" width="100%">
</p>
<p align="center">
Figure 1: Synchronization control mode diagram
</p>

<table border="1" align="center">
  <tr bgcolor="lightgray">
    <td>Synchronization Control Mode</td>
    <td align="center">Description</td>
  </tr>
  <tr>
    <td rowspan="2">mode 0</td>
    <td>For AIC scenarios, synchronize all AIC cores. Instructions after <code>CrossCoreWaitFlag</code> execute only when all AIC cores have reached <code>CrossCoreSetFlag</code>.</td>
  </tr>
  <tr>
    <td>For AIV scenarios, synchronize all AIV cores. Instructions after <code>CrossCoreWaitFlag</code> execute only when all AIV cores have reached <code>CrossCoreSetFlag</code>.</td>
  </tr>
  <tr>
    <td>mode 1</td>
    <td>Synchronization control between AIV cores within a single AI Core. Instructions after <code>CrossCoreWaitFlag</code> execute only when both AIV cores have run <code>CrossCoreSetFlag</code>.</td>
  </tr>
  <tr>
    <td rowspan="2">mode 2</td>
    <td>After the AIC core executes <code>CrossCoreSetFlag</code>, instructions after <code>CrossCoreWaitFlag</code> on the two AIVs continue to execute.</td>
  </tr>
  <tr>
    <td>After both AIVs execute <code>CrossCoreSetFlag</code>, instructions after <code>CrossCoreWaitFlag</code> on the AIC can execute.</td>
  </tr>
  <tr>
    <td>mode 4</td>
    <td>Synchronizes the AIC with one selected AIV in an AI Core. AIV0 and AIV1 use different <code>flagId</code> mappings when synchronizing with the AIC.</td>
  </tr>
</table>

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR&950DT products | >= CANN 9.2.0 |
| Atlas A3 products | >= CANN 9.2.0 |
| Atlas A2 products | >= CANN 9.2.0 |

## Directory Structure

```
├── cross_core_set_wait_flag
│   ├── scripts
│   │   ├── gen_data.py              // Script for generating input data and ground truth data
│   │   └── verify_result.py         // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt               // Build project file
│   ├── data_utils.h                 // Data read/write functions
│   ├── figures                      // Illustrations
│   ├── cross_core_set_wait_flag.h   // Ascend C example implementation
│   ├── cross_core_set_wait_flag.asc // Invocation example and result verification
│   └── README.md                    // Example documentation
```

## Example Description
<table border="1" style="text-align: center;">
  <tr>
    <td>SCENARIO_NUM Value</td>
    <td>Business Scenario</td>
    <td>Synchronization Mode Used</td>
  </tr>
  <tr>
    <td>0</td>
    <td>Pure Vector computation scenario (16 AIVs)</td>
    <td>mode 0 (all AIV core synchronization)</td>
  </tr>
  <tr>
    <td>1</td>
    <td>Pure Vector computation scenario (2 AIVs)</td>
    <td>mode 1</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Cube and Vector fused computation scenario</td>
    <td>mode 2 (AIC waits for AIV), mode 2 (AIV waits for AIC), mode 0 (all AIC core synchronization)</td>
  </tr>
  <tr>
    <td>3</td>
    <td>Cube and Vector fused computation (Ascend 950PR/Ascend 950DT only)</td>
    <td>mode 4 (AIC synchronizes independently with AIV0 and AIV1)</td>
  </tr>
</table>
`SCENARIO_NUM` controls the execution branch:

- Scenarios 0 to 2 can be built for NPU architecture `dav-2201` and NPU architecture `dav-3510`.
- Scenario 3 uses mode 4 and the L0C Buffer-to-UB data path, so it requires NPU architecture `dav-3510`.

### Computation Formula and Example Specifications

#### `SCENARIO_NUM=0` (Pure Vector Computation Scenario, Mode 0)
- Computation formula:  
  $$
  z = \sum_{i=0}^{15} (x \times i)
  $$
  - `x` is the input vector, all ones.
  - `i` is the `BlockIdx` of each AIV (range 0-15).
  - `z` is the accumulated value of computation results from all AIVs.

- Example specifications:
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"><code>CrossCoreSetFlagMode0</code></td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">cross_core_set_wait_flag_custom</td></tr>
  </table>

#### `SCENARIO_NUM=1` (Pure Vector Computation Scenario, Mode 1)
- Computation formula:  
  $$
  z = (x \times 2) + (x \times 3)
  $$
  - `x` is the input vector, all ones.
  - Only AIVs with `BlockIdx=2` and `BlockIdx=3` participate in computation.
  - `z` is the accumulated value of computation results from these two AIVs.

- Example specifications:
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"><code>CrossCoreSetFlagMode1</code></td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[32]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">cross_core_set_wait_flag_custom</td></tr>
  </table>

#### `SCENARIO_NUM=2` (Cube and Vector Fused Computation Scenario)
- Computation formula:  
  $$
  C = \text{LeakyRelu}(Cast(A) \times Cast(B))
  $$
  - `A` is the left matrix with shape `[M, K]` and data type `uint8`.
  - `B` is the right matrix with shape `[K, N]` and data type `uint8`.
  - First convert the data types of `A` and `B` from `uint8` to `half`.
  - Then perform matrix multiplication: `A × B`.
  - Finally perform `LeakyRelu` operation on the result.
  - `C` is the final result with shape `[M, N]` and data type `float32`.

- Example specifications:
  <table border="1" align="center">
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center"><code>CrossCoreSetFlagMode2</code></td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[32, 32]</td><td align="center">uint8</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[32, 64]</td><td align="center">uint8</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[32, 64]</td><td align="center">float32</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">mmad_custom</td></tr>
  </table>

#### `SCENARIO_NUM=3` (Cube and Vector Fused Computation, Mode 4)

Scenario 3 uses the same mathematical formula and tensor specifications as scenario 2, but the computation organization is different:

- Scenario 2 splits the K dimension across multiple AICs, so the partial matrix multiplication results from each AIC need to be atomically accumulated on GM.
- Scenario 3 launches only one AI Core. A single AIC processes the complete K dimension, so the AIC already obtains the complete matrix multiplication result. It does not use mode 0 all-AIC synchronization and does not perform atomic accumulation of multi-AIC block results. AIV0 and AIV1 convert matrices A and B and independently notify the AIC through mode 4. The first 16 rows of the matrix multiplication result move directly from L0C Buffer to AIV0's UB through Fixpipe. The remaining 16 rows move from L0C Buffer to GM and then from GM to AIV1's UB. The two AIVs apply `LeakyRelu` and write back their corresponding GM regions.

In mode 4, `flagId` has an offset mapping between AIV1 and AIC:

- When AIV1 uses 0-15, the AIC side corresponds to 16-31.
- When the AIC uses 16-31, the AIV1 side corresponds to 0-15.

For details, see the [CrossCoreSetFlag flagId rules](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md#flagId取值范围说明).

## Example Implementation
### 1. Cube and Vector Fused Computation Scenario
#### 1.1 Overall Logic
<p align="center">
  <img src="figures/融合场景_示意图.png" width="100%">
   </p>
<p align="center">
Figure 2: Cube and Vector fused computation scenario, overall computation logic diagram
</p>

This example focuses on the fused operator scenario (configured with `__mix__(1, 2)`), where each AI Core contains 1 AIC and 2 AIVs. The configured logical core count `numBlocks = 8`, corresponding to 8 AICs and 16 AIVs.

As shown in Figure 2, the overall computation logic is divided into three core phases: precision conversion phase, block matrix multiplication and atomic accumulation phase, and `LeakyRelu` operation and result write-back phase.
#### 1.2 Precision Conversion Phase (Mode 2, AIC Waits for AIV Within a Single AI Core)
Since the data type of the left and right matrices on GM is `uint8`, which does not meet the input data type requirements of the `mmad` instruction, the data on GM must first be transferred to AIV for precision conversion before block matrix multiplication can be performed in AIC. Therefore,
within each AI Core, one AIC needs to wait for the other 2 AIVs within the same AI Core to complete data precision conversion before starting block matrix multiplication computation.
Specifically: the left matrix (A matrix) data in GM is split into 8 parts along the K axis, assigned to AIVs with even `BlockIdx` for `uint8` to `half` precision conversion; the right matrix (B matrix) data in GM is split into 8 parts along the K axis, assigned to AIVs with odd `BlockIdx` for `uint8` to `half` precision conversion. As shown in Figure 3, based on the above description, inter-core synchronization mode 2 is required.
The code segment corresponding to the above description is as follows:

```cpp
if (blockIdx % 2 == 0) {
    AscendC::DataCopy(aLocal, aGM, A_BLOCKS_LENGTH);
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::Cast(castALocal, aLocal, AscendC::RoundMode::CAST_NONE, A_BLOCKS_LENGTH);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::DataCopy(AVectorGM, castALocal, A_BLOCKS_LENGTH);
} else {
    AscendC::DataCopy(bLocal, bGM, B_BLOCKS_LENGTH);
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::Cast(castBLocal, bLocal, AscendC::RoundMode::CAST_NONE, B_BLOCKS_LENGTH);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::DataCopy(BVectorGM, castBLocal, B_BLOCKS_LENGTH);
}
// Mode 2: within each AI Core, one AIC waits for 2 AIVs
AscendC::CrossCoreSetFlag<2, PIPE_MTE3>(SYNC_AIV_AIC_FLAG);
```

<p align="center">
  <img src="figures/融合场景_精度转换阶段.png" width="100%">
   </p>
<p align="center">
Figure 3: Precision conversion phase, mode 2 diagram
</p>

#### 1.3 Block Matrix Multiplication and Atomic Accumulation Phase (Mode 0, All AIC Core Synchronization)
After each AIC performs block matrix multiplication computation, it enables the atomic accumulation summation mechanism, transfers the computation results to the same GM region, and accumulates the block matrix multiplication results from 8 AICs on GM to obtain the complete C matrix. To obtain the correct C matrix, it is necessary to wait for all 8 AICs to complete block matrix multiplication and transfer results to GM via `FixPipe`. As shown in Figure 4, based on the above description, inter-core synchronization mode 0 (all AIC core synchronization) is required. The code segment corresponding to the above description is as follows:
$$
C = \sum_{i=1}^{8} A_i \cdot B_i
$$

```cpp
// Mode 2: within each AI Core, AIC waits for 2 AIVs
CrossCoreWaitFlagForArch<2, PIPE_MTE2>(SYNC_AIV_AIC_FLAG);

CopyIn(a1Local, b1Local);
SplitA(a1Local, a2Local);
SplitBTranspose(b1Local, b2Local);
Compute(a2Local, b2Local, c1Local);
CopyOut(c1Local);
// Mode 0: all 8 AICs synchronize to ensure correct atomic accumulation results.
AscendC::CrossCoreSetFlag<0, PIPE_FIX>(SYNC_AIC_FLAG);
CrossCoreWaitFlagForArch<0, PIPE_FIX>(SYNC_AIC_FLAG);

// Mode 2: the AIC notifies 2 AIVs in the same AI Core to execute LeakyRelu.
AscendC::CrossCoreSetFlag<2, PIPE_FIX>(SYNC_AIC_AIV_FLAG);
```

<p align="center">
  <img src="figures/融合场景_分块矩阵乘与原子累加阶段.png" width="100%">
   </p>
<p align="center">
Figure 4: Block matrix multiplication and atomic accumulation phase, mode 0 diagram
</p>

#### 1.4 LeakyRelu and Result Write-Back Phase (Mode 2, 2 AIVs Wait for AIC)
Within each AI Core, 2 AIVs need to wait for AIC to complete block matrix multiplication and atomic accumulation operations before performing `LeakyRelu` operation on C matrix blocks.
Specifically, the accumulated C matrix is split into 16 parts along the M axis, assigned to 16 AIVs for `LeakyRelu` computation respectively. As shown in Figure 5, based on the above description, inter-core synchronization mode 2 (2 AIVs wait for one AIC within a single AI Core) is required. The code segment corresponding to the above description is as follows:

```cpp
// Mode 2: within each AI Core, 2 AIVs wait for AIC
CrossCoreWaitFlagForArch<2, PIPE_MTE2>(SYNC_AIC_AIV_FLAG);
AscendC::DataCopy(cLocal, CVectorGM, C_AIV_BLOCKS_LENGTH);
AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

// Perform LeakyRelu operation
float alpha = 0.001;
AscendC::LeakyRelu(reluCLocal, cLocal, alpha, C_AIV_BLOCKS_LENGTH);
AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
AscendC::DataCopy(CVectorGM, reluCLocal, C_AIV_BLOCKS_LENGTH);
```

<p align="center">
  <img src="figures/融合场景_LeakyRelu运算与结果回写阶段.png" width="100%">
   </p>
<p align="center">
Figure 5: LeakyRelu operation and result write-back phase, mode 2 diagram
</p>

#### 1.5 Split L0C-to-UB/GM Output Phase (Mode 4)

Scenario 3 sets `FUSED_NUM_BLOCKS` to 1, and `AIC_K` on the AIC side equals the complete K dimension. Therefore, the matrix multiplication result produced by the single AIC is consistent with the complete matrix multiplication result obtained by multi-AIC block accumulation in scenario 2. After matrix multiplication, the AIC sends two mode 4 notifications to wake up AIV0 and AIV1 separately. AIV0 directly consumes the first half of the matrix from L0C Buffer to UB, while AIV1 loads the second half from GM. The `LeakyRelu` inputs obtained by the two AIVs together cover the complete C matrix. This demonstrates both single-AIV synchronization and the L0C Buffer-to-UB data path available on Ascend 950PR/Ascend 950DT.

```cpp
// AIV0/AIV1 notify the AIC that inputs are ready.
AscendC::CrossCoreSetFlag<4, PIPE_MTE3>(SYNC_MODE4_INPUT_FLAG);

// The AIC waits for AIV0 and AIV1 to complete precision conversion. It uses flagId + 16 when waiting for AIV1.
AscendC::CrossCoreWaitFlag<4, PIPE_MTE2>(SYNC_MODE4_INPUT_FLAG);
AscendC::CrossCoreWaitFlag<4, PIPE_MTE2>(SYNC_MODE4_INPUT_FLAG + SYNC_MODE4_AIV1_OFFSET);

// The AIC notifies AIV0 and AIV1 that outputs are ready.
AscendC::CrossCoreSetFlag<4, PIPE_FIX>(SYNC_MODE4_OUTPUT_FLAG);
AscendC::CrossCoreSetFlag<4, PIPE_FIX>(SYNC_MODE4_OUTPUT_FLAG + SYNC_MODE4_AIV1_OFFSET);
```

### 2. Pure Vector Computation Scenario
#### 2.1 Comparison of Mode 0 and Mode 1
This example sets `NUM_BLOCKS` to 8 (8 AI Cores), with each AI Core having an AIC to AIV ratio of 1:2, meaning this example launches 8 AICs and 16 AIVs in total, with AIV `BlockIdx` ranging from 0 to 15.
As shown in Figure 6 below, the computation logic of mode 0 and mode 1 in this example is nearly identical, with the only difference being the number of AIVs participating in synchronization: in mode 0, all 16 AIVs participate in synchronization; in mode 1, only 2 AIVs (`BlockIdx=2` and `BlockIdx=3`) in the second AI Core participate in synchronization. Therefore, the next section will detail the overall logic of mode 0, and mode 1 will not be described in detail.
<p align="center">
  <img src="figures/纯aiv_模式1和模式0的区别.png" width="100%">
</p>
<p align="center">
Figure 6: Pure AIV scenario, difference between mode 0 and mode 1 diagram
</p>

#### 2.2 Overall Logic of Mode 0
The GM used in this example is divided into 2 blocks: one for storing input data (`initialDataGm`) and one for storing the accumulated results from all AIVs (`atomicResultGm`).
As shown in Figure 7 below, the overall logic of mode 0 is divided into the following steps:
(1) Each AIV transfers all-ones data from `initialDataGm` to UB, which is a `PIPE_MTE2` pipeline operation.

(2) Each AIV performs vector computation on the data in UB: multiply by the `BlockIdx` corresponding to each core using the `Muls` instruction, which is a `PIPE_V` pipeline operation.

(3) Step 3 corresponds to the following code snippet. `atomicResultGm` is used to store the accumulated results after all 16 AIVs complete transfer. By calling `CrossCoreSetFlag` and `CrossCoreWaitFlag` interfaces, synchronization control is achieved: ensuring that after all 16 AIVs complete the `PIPE_MTE3` transfer instruction, instructions after `CrossCoreWaitFlag` can execute.

```cpp
// Enable atomic accumulation for UB to GM transfer: data transferred to atomicResult is accumulated with the original value and overwrites the original value.
AscendC::SetAtomicAdd<float>();
// DataCopy is a PIPE_MTE3 pipeline operation.
AscendC::DataCopy(atomicResultGm, xLocal, this->blockLength);
// After this AIV completes the preceding PIPE_MTE3(DataCopy) pipeline operation, notify other AIV cores that this AIV has completed.
AscendC::CrossCoreSetFlag<0, PIPE_MTE3>(0);
// Block this AIV from continuing to execute instructions until all other AIVs complete the PIPE_MTE3 pipeline operation, then unblock and continue execution.
CrossCoreWaitFlagForArch<0, PIPE_MTE2>(0);
// Disable atomic accumulation.
AscendC::DisableDmaAtomic();
```

After the above synchronization is complete, `atomicResultGm` already contains the accumulated value of vector computation results from 16 AIVs.
If the synchronization in the previous step is not inserted correctly, the data transferred from `atomicResultGm` to AIV may be the accumulated value of vector computation results from only some AIVs, resulting in inaccurate results.

```cpp
if (AscendC::GetBlockIdx() == 0) {
    AscendC::DataCopy(yLocal, atomicResultGm, this->blockLength);   // PIPE_MTE2
    AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
    AscendC::DataCopy(atomicResultGm, yLocal, this->blockLength);
    return;
}
```
<p align="center">
  <img src="figures/纯aiv_模式0示意图.png" width="100%">
</p>
<p align="center">
Figure 7: Pure AIV scenario, mode 0 computation logic diagram
</p>

### 3. Notes
#### 3.1 Cube and Vector Fused Computation Scenario
(1) In the Cube and Vector fused computation scenario, the fused operator (configured with `__mix__(1, 2)`) requires `ASCEND_IS_AIV`/`ASCEND_IS_AIC` for code isolation between AIV and AIC cores.
```cpp
AscendC::InitSocState();
KernelMmad op;
if ASCEND_IS_AIC {
    op.InitAIC(A, B, c);
    op.ProcessAIC();
} 
if ASCEND_IS_AIV {
    op.InitAIV(a, b, A, B, c);
    op.ProcessAIV();
}
AscendC::PipeBarrier<PIPE_ALL>();
```
(2) The return value of `GetBlockIdx` (obtaining the index of the current core) has different value ranges in AIC and AIV, and its values are related to the logical core count set by the operator and the AIC to AIV ratio in one AI Core. In this example, `NUM_BLOCKS=8` and the AIC to AIV ratio is 1:2, so the return value of `GetBlockIdx` ranges from 0-7 in AIC and 0-15 in AIV.

(3) The example uses the static Tensor programming paradigm, which requires manually inserting intra-core synchronization. Additionally, in the static Tensor programming mode, developers need to manually call the `InitSocState()` interface to initialize the global state register.

#### 3.2 Inter-Core Synchronization Constraints

(1) `CrossCoreSetFlag` and `CrossCoreWaitFlag` must be paired. See the [CrossCoreSetFlag constraints](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md#section633mcpsimp) and [CrossCoreWaitFlag constraints](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md#section633mcpsimp) for supported modes, kernel types, pipelines, and `flagId` ranges.

(2) Mode 4 supports only Ascend 950PR/Ascend 950DT (corresponding to NPU architecture `dav-3510`) and requires a `__mix__(1, 2)` kernel.

(3) The `pipe` template parameter of `CrossCoreWaitFlag` does not affect the hardware instruction on NPU architecture `dav-2201`, but takes effect on NPU architecture `dav-3510`. Because NPU architecture `dav-3510` does not support the default `PIPE_S` for modes 0, 1, and 2, the example uses architecture-specific handling: it passes `PIPE_MTE2` or `PIPE_FIX` explicitly on NPU architecture `dav-3510` and keeps the default parameter behavior on NPU architecture `dav-2201`. See [CrossCoreWaitFlag template parameter defaults and effects](../../../../../docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md#template-parameter-defaults) for details.

To support both NPU architectures described above, this example encapsulates the `CrossCoreWaitFlagForArch` function. The function explicitly passes the `pipe` template parameter on NPU architecture `dav-3510`, while omitting it and using the default value on other NPU architectures. The implementation is as follows:

```cpp
template <uint8_t modeId, pipe_t pipe>
__aicore__ inline void CrossCoreWaitFlagForArch(uint16_t flagId)
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
    AscendC::CrossCoreWaitFlag<modeId, pipe>(flagId);
#else
    AscendC::CrossCoreWaitFlag<modeId>(flagId);
#endif
}
```

#### 3.3 Pure Vector Computation Scenario
(1) When using the `CrossCoreSetFlag` and `CrossCoreWaitFlag` inter-core synchronization interfaces, even in pure Vector computation scenarios, the kernel function cannot use the `__vector__` modifier.
The kernel function in this example uses `__mix__(1, 2)` decoration, but in the pure Vector scenario, since only vector computation is performed, the `ASCEND_IS_AIV` macro must be used to ensure the program runs only on AIV cores, otherwise the program will hang.
```cpp
AscendC::InitSocState();
KernelCrossCoreSetFlag op;
if ASCEND_IS_AIV {
    op.Init(x, z, dataLength);
    op.Process();
}
AscendC::PipeBarrier<PIPE_ALL>();
```
 (2) Mode 1 requires that the 2 AIVs participating in synchronization must belong to the same AI Core, otherwise the program will hang. In this example, the return values of `GetBlockIdx` for the two AIVs participating in synchronization are 2 and 3, which belong to the 2nd AI Core (index starting from 1); if the return values of `GetBlockIdx` are changed to 3 and 4 (belonging to two different AI Cores), the program will hang.

 (3) The example uses the static Tensor programming paradigm, which requires manually inserting intra-core synchronization. Additionally, in the static Tensor programming mode, developers need to manually call the `InitSocState()` interface to initialize the global state register.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
    
- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=0  # Set the scenario number (values: 0, 1, 2, 3)
  ASC_ARCH=dav-2201  # Set the NPU architecture. Scenario 3 requires NPU architecture dav-3510
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=$ASC_ARCH -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=$ASC_ARCH -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=$ASC_ARCH -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: NPU architecture `dav-2201` corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, and NPU architecture `dav-3510` corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0` (default), `1`, `2`, `3` | Scenario number: 0 (pure Vector mode 0), 1 (pure Vector mode 1), 2 (Cube+Vector modes 0/2), 3 (Cube+Vector mode 4, NPU architecture `dav-3510` only) |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.
  ```bash
  test pass!
  ```

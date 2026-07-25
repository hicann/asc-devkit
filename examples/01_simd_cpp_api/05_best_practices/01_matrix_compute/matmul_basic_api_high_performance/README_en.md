# Matmul Basic API Best Practice Example

## Overview

This example is based on the static Tensor programming paradigm, implementing high-performance matrix multiplication through multiple optimization methods including L1/L0 double buffering, large packet transfer, fine-grained pipeline synchronization, UnitFlag, and L2Cache. This example is implemented based on the basic API, using the same optimization methods as the high-level API version. The example goal is to demonstrate tuning implementation details based on the static Tensor programming paradigm.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── matmul_basic_api_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Ground truth comparison script
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── mmad.asc                // Ascend C example implementation
│   └── README.md               // Example documentation
```

## Example Description

  Matmul computation formula:
  $$
  C = A * B
  $$
  - A, B are source operands. A is the left matrix, shape [M, K]; B is the right matrix, shape [K, N]
  - C is the destination operand, storing the matrix multiplication result matrix, shape [M, N]

- Example specifications:

<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center">Matmul</td></tr>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
<tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
<tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">true</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">mmad_custom</td></tr>
</table>

## Example Implementation

### Performance Metric Description

| Metric | Description |
|------|------|
| Task Duration(μs) | Total execution time of the entire task, operator execution time is determined by this parameter |
| Block Num | Number of cores used (Block count) |
| aicore_time(μs) | Average AI Core execution time |
| aic_mac_time(μs) | Cube computation unit execution time |
| aic_mac_ratio | Cube computation unit time ratio, reflecting computation unit utilization |
| aic_mte1_time(μs) | MTE1 (L1 to L0A/L0B transfer) execution time |
| aic_mte1_ratio | MTE1 time ratio, reflecting L1 to L0 data transfer pressure |
| aic_mte2_time(μs) | MTE2 ([GM](../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMD编程/抽象硬件架构.md) (Global Memory) to L1 transfer) execution time |
| aic_mte2_ratio | MTE2 time ratio, reflecting GM to L1 data loading pressure |
| aic_fixpipe_time(μs) | [Fixpipe](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md) (L0C to GM transfer) execution time |
| aic_fixpipe_ratio | [Fixpipe](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md) time ratio, reflecting result write-back memory access pressure |


### Data Flow Path:

```
GM ──(MTE2, DataCopy)──> L1 ──(MTE1, LoadData)──> L0A/L0B ──(Cube, Mmad)──> L0C ──(Fixpipe)──> GM
       DataCopyInA/B             DataLoadA/B                   Compute             CopyOut
```

### Intermediate Performance Optimization

#### 1. L1/L0 Double Buffer Ping-Pong Layout

Both L1 and L0 use Ping-Pong double buffering, forming a three-stage pipeline with DataCopyIn (GM→L1), DataLoad (L1→L0), and Compute stages. Each stage processes data from different buffers without blocking each other.

```
Time ──────────────────────────────────────────────────────────────>

MTE2:  |─ A1 Ping ──|─ A1 Pong ──|─ A1 Ping ──| ...
MTE1:               |─ A2 Ping ──|─ A2 Pong ──|─ A2 Ping ──| ...
Cube:                            |─ Mmad ─────|─ Mmad ─────| ...
Fixpipe:                           |─ CopyOut ──|(unitflag)
```

**L1 Double Buffer Layout**: A1 occupies the first half of L1 (0~256KB), B1 occupies the second half (256~512KB), each further divided into Ping/Pong:

```
L1 (512KB):
├── A1 Ping: [0, 128KB)
├── A1 Pong: [128KB, 256KB)
├── B1 Ping: [256KB, 384KB)
└── B1 Pong: [384KB, 512KB)
```

**L0 Double Buffer Layout**: A2/B2 each have independent 64KB space, each further divided into Ping/Pong:

```
L0A/L0B (64KB):
├── A2 Ping: [0, 16KB)
├── A2 Pong: [32KB, 48KB)
├── B2 Ping: [0, 32KB)
└── B2 Pong: [32KB, 64KB)
```

```cpp
// A1: L1 Ping/Pong
AscendC::LocalTensor<half> a1LocalPing(AscendC::TPosition::A1, 0, a1PingpongSize);
AscendC::LocalTensor<half> a1LocalPong(AscendC::TPosition::A1, a1PingpongSize * sizeof(half), a1PingpongSize);
// A2: L0 Ping/Pong
AscendC::LocalTensor<half> a2LocalPing(AscendC::TPosition::A2, 0, a2PingpongSize);
AscendC::LocalTensor<half> a2LocalPong(AscendC::TPosition::A2, L0_PINGPONG_BYTES, a2PingpongSize);
```

#### 2. Fine-Grained Pipeline Synchronization

Four types of hardware event flags are used to implement precise pipeline synchronization, divided into forward synchronization (data readiness notification) and reverse synchronization (buffer release notification):

| Event Type | Direction | Purpose | Flag Number |
|---------|------|------|----------|
| [MTE2_MTE1](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) | Forward | L1 data readiness notification, DataCopyIn notifies DataLoad that data can be read | 0/1: A1 Ping/Pong; 2/3: B1 Ping/Pong |
| [MTE1_MTE2](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) | Reverse | L1 buffer release notification, DataLoad notifies DataCopyIn that buffer can be written | Same as above |
| [MTE1_M](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) | Forward | L0 data readiness notification, DataLoad notifies Compute that computation can start | mte1DBFlag (0/1 alternating) |
| [M_MTE1](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) | Reverse | L0 buffer release notification, Compute notifies DataLoad that buffer can be written | mte1DBFlag (0/1 alternating) |

**Reverse synchronization must be preset**: Since reverse synchronization is "consumer [SetFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) → producer [WaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)", SetFlag must be preset before first use, otherwise the first WaitFlag will deadlock:

```cpp
// Initialization: Preset reverse synchronization flags to prevent first WaitFlag deadlock
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID0);  // A1 Ping writable
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID1);  // A1 Pong writable
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID2);  // B1 Ping writable
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID3);  // B1 Pong writable
AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID0);     // L0 Ping writable
AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID1);     // L0 Pong writable
```

**Large packet granularity forward synchronization**: DataLoad reads data from L1 large packets one base block at a time along K direction, only needing to wait for data readiness when reading the first base block of the large packet. Subsequent base blocks are in the same large packet as the first, no repeated waiting needed:

```cpp
// Forward synchronization: Only the first element of the large packet needs to wait for data readiness
if (kOffsetInChunkA == 0) {
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(a1ReadIdx);
}
// Reverse synchronization: Notify DataCopyIn that buffer can be overwritten only after the last element of the large packet is consumed
if ((kOffsetInChunkA + 1) == stepKa) {
    AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(a1ReadIdx);
}
```

#### 3. Multi-Core Parallel Splitting

Split the matrix evenly along the M/N directions for parallel computation across multiple cores. On Atlas A2/A3 (`dav-2201`), use a 4×6 splitting strategy (4 blocks in the M direction and 6 blocks in the N direction, 24 cores in total) to satisfy 512B address alignment and reduce same-address access conflicts. On Ascend 950PR/Ascend 950DT (`dav-3510`), use a 32-core split (see the performance data section below):

```cpp
constexpr uint32_t mIter = DivCeil(M, singleCoreM);
uint32_t mIterIdx = AscendC::GetBlockIdx() % mIter;
uint32_t nIterIdx = AscendC::GetBlockIdx() / mIter;
```

#### 4. DataCopyIn Prioritizes B Matrix Transfer

When `stepKa > stepKb`, B matrix needs to switch to the next L1 buffer (Pong) every `stepKb` baseK blocks, while A matrix switches every `stepKa` baseK blocks. Therefore B's data demand is more urgent. If A is transferred first, the MTE2 pipeline is occupied by A, and B's transfer must wait until A finishes, causing B data to not be ready when needed.

This example triggers DataCopyIn after Compute, **B first then A**, prioritizing the more urgent B data transfer:

```
k=0:  Compute → DataCopyIn(B1 Pong) → DataCopyIn(A1 Pong)
                 ↑ MTE2 transfers B first  ↑ Then transfers A
k=stepKb:       Needs B1 Pong → Already ready ✓ (B has had stepKb rounds to transfer)
k=stepKa:       Needs A1 Pong → Already ready ✓ (A has stepKa rounds to transfer, more ample)
```

### Advanced Extreme Refinement

#### 5. Large Packet Transfer

Through `stepKa`/`stepKb` parameters, multiple basic blocks are packed into one DataCopyIn operation (called "large packet"), reducing MTE2 transfer count. For example, `stepKa=8` means transferring 8 baseM * baseK blocks from GM to L1 at once.

```cpp
// DataCopyInA: Transfer stepKa baseK blocks at once
AscendC::Nd2NzParams nd2nzParams;
nd2nzParams.nValue = curM;
nd2nzParams.dValue = baseK * stepKa;  // Large packet contains stepKa baseM * baseK blocks
```

#### 6. LoadData3D Replacing LoadData2D — Reducing Instruction Queue Usage

On Atlas A2/A3 architecture, this example uses [`LoadData3DParamsV2`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_3D.md) (that is, [LoadData3D](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_3D.md)) to replace [`LoadData2DParams`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md) (that is, LoadData2D) for L1→L0 data transfer. This is a key instruction queue optimization.

**Problem Background**: MTE1 instruction queue depth is 32. When using LoadData2D, due to limited single instruction transfer granularity, transferring one baseM×baseK slice requires a for loop to dispatch multiple LoadData2D instructions. For example, with baseM=128 and baseK=64, at least `baseK/16 = 4` LoadData2D instructions need to be dispatched.

**Code Implementation**: When using LoadData2D, multiple transfer instructions need to be dispatched in a loop at `CUBE_BLOCK` granularity; after switching to LoadData3D, one instruction can describe the complete base slice.

Before optimization, with `baseK=64`, loop count is 4:

```cpp
AscendC::LoadData2DParams loadDataParams;
for (int i = 0; i < DivCeil(baseK, CUBE_BLOCK); ++i) {
    AscendC::LoadData(b2Local[i * dstOffset], b1Local[srcAddr + i * srcOffset], loadDataParams);
}
```

After optimization, use `LoadData3DParamsV2` to describe the `baseM * baseK` slice at once:

```cpp
AscendC::LoadData3DParamsV2<half> loadDataParams;
loadDataParams.l1H = 1;
loadDataParams.l1W = baseM;
loadDataParams.channelSize = baseK;
loadDataParams.kExtension = baseK;
loadDataParams.mExtension = curMAlign;
loadDataParams.mStartPt = 0;
loadDataParams.kStartPt = 0;
AscendC::LoadData(a2Local, a1Local[srcAddr], loadDataParams);
```

This reduces MTE1 instruction count per base slice from multiple `LoadData2D` to 1 `LoadData3D`, reducing MTE1 queue usage. For non-transposed B matrix input, transposed transfer is also completed through `LoadData3DParamsV2` with `enTranspose = true` configuration.

> **Notice**: Atlas A5 chip provides the `LoadData2DParamsV2` interface, completing transfer with a single instruction without needing LoadData3D. Therefore, this example uses `__NPU_ARCH__` conditional compilation to distinguish LoadData implementations for the two architectures.

#### 7. Constant Tiling

All Tiling parameters (baseM/baseK/baseN, singleCoreM/K/N, stepKa/stepKb) are determined at compile time through template parameters, requiring no Scalar dynamic computation at runtime, reducing Scalar overhead:

```cpp
template <uint32_t M, uint32_t K, uint32_t N, uint32_t baseM, uint32_t baseK, uint32_t baseN,
          uint32_t singleCoreM, uint32_t singleCoreK, uint32_t singleCoreN,
          uint32_t stepKa, uint32_t stepKb>
class KernelMmad { ... };
```

#### 8. UnitFlag Optimization

After enabling UnitFlag, [MMAD](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md) and [FIXPIPE](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md) achieve fine-grained (512B) pipeline parallelism instead of instruction-level synchronization. Whenever Cube completes computation of one 512B data result, FIXPIPE immediately transfers that data, with Cube computation and result write-back pipeline overlapping:

```cpp
mmadParams.unitFlag = (kBlockIdx != kLoopCount - 1) ? 2 : 3;  // Enable UnitFlag
```

- `unitFlag = 2`: Intermediate K block, MMAD computation result is not immediately written back, but waits for the next 512B completion before pipeline transfer
- `unitFlag = 3`: Last K block, notifies FIXPIPE to write all remaining results back to GM

#### 9. L2Cache Optimization

L2Cache is the shared external cache for AI Cores, with pure read bandwidth approximately 3 to 4 times that of GM. If data cannot hit L2Cache and needs to access GM, bandwidth utilization efficiency is low, causing MTE2 to become a performance bottleneck.

The specific L2Cache splitting implementation is consistent with Case 6 in the [high-level API Matmul example](../matmul_high_performance/README.md), with the core idea being to split the A matrix along the M axis to make the B matrix reside in L2Cache across rounds. This example implements a dual outer loop through the `ProcessL2Cache()` method, scheduling by `outerMIdx` rounds, with 24 cores computing their respective sub-blocks in parallel within each round:

```cpp
// ProcessL2Cache: Split by M direction in rounds, each round 24 cores cover mIterPerRound M sub-blocks
constexpr uint32_t mIterPerRound = DivCeil(M, singleCoreM * 2);
constexpr uint32_t outerMLoopCount = DivCeil(mIterTotal, mIterPerRound);

for (uint32_t outerMIdx = 0; outerMIdx < outerMLoopCount; outerMIdx++) {
    uint32_t mIterIdx = AscendC::GetBlockIdx() % mIterPerRound + outerMIdx * mIterPerRound;
    uint32_t nIterIdx = AscendC::GetBlockIdx() / mIterPerRound;
    if (mIterIdx >= mIterTotal || nIterIdx >= nIterTotal) continue;
    InitComputeParamsL2Cache(mIterIdx, nIterIdx);
    ProcessLoop(...);
}
```

#### K Direction Main Loop Complete Flow

Using stepKa=8, stepKb=4 as example, showing the detailed K direction loop execution flow within one (mBlockIdx, nBlockIdx) sub-block:

```
Preprocessing:
  SetFlag(MTE1_MTE2, 0/1/2/3)  // Preset reverse sync: L1 Ping/Pong all writable
  SetFlag(M_MTE1, 0/1)         // Preset reverse sync: L0 Ping/Pong all writable
  DataCopyIn(A1 Ping, k=0)     // Transfer first A large packet to Ping
  DataCopyIn(B1 Ping, k=0)     // Transfer first B large packet to Ping
  SetFlag(MTE2_MTE1, 0/2)      // Notify A1/B1 Ping data ready

K loop kBlockIdx = 0, 1, ..., kLoopCount-1:
  ┌─ a1ReadIdx = (kBlockIdx / stepKa) % 2        // Current L1 A Ping/Pong read index
  │  b1ReadIdx = (kBlockIdx / stepKb) % 2        // Current L1 B Ping/Pong read index
  │  kOffsetInChunkA = kBlockIdx % stepKa        // Current baseK offset within A large packet
  │  kOffsetInChunkB = kBlockIdx % stepKb        // Current baseK offset within B large packet
  │
  │  WaitFlag(M_MTE1, mte1DBFlag)                // Wait for previous Compute to release L0
  │  if (kOffsetInChunkA == 0)
  │      WaitFlag(MTE2_MTE1, a1ReadIdx)          // Wait for L1 A large packet data ready (first element only)
  │  if (kOffsetInChunkB == 0)
  │      WaitFlag(MTE2_MTE1, b1ReadIdx + 2)      // Wait for L1 B large packet data ready (first element only)
  │
  │  DataLoadA(A1 → A2)                          // L1 → L0
  │  DataLoadB(B1 → B2)                          // L1 → L0
  │
  │  if (kOffsetInChunkA + 1 == stepKa)
  │      SetFlag(MTE1_MTE2, a1ReadIdx)           // A large packet last element: Notify L1 A writable
  │  if (kOffsetInChunkB + 1 == stepKb)
  │      SetFlag(MTE1_MTE2, b1ReadIdx + 2)       // B large packet last element: Notify L1 B writable
  │
  │  Compute(Mmad)                               // Cube computation (M instruction)
  │  SetFlag(M_MTE1, mte1DBFlag)                 // Notify L0 can be overwritten
  │  mte1DBFlag ^= 1                             // Switch L0 Ping/Pong
  │
  │  // DataCopyIn placed after Compute, B first then A
  │  if (B large packet last element && more B data):
  │      WaitFlag(MTE1_MTE2, b1WriteIdx + 2)     // Wait for L1 B buffer writable
  │      DataCopyInB(next B large packet)         // GM → L1 (MTE2 instruction)
  │      SetFlag(MTE2_MTE1, b1WriteIdx + 2)      // Notify L1 B data ready
  │  if (A large packet last element && more A data):
  │      WaitFlag(MTE1_MTE2, a1WriteIdx)         // Wait for L1 A buffer writable
  │      DataCopyInA(next A large packet)         // GM → L1 (MTE2 instruction)
  │      SetFlag(MTE2_MTE1, a1WriteIdx)          // Notify L1 A data ready
  └─
```

**Timing example** (`stepKa=8`, `stepKb=4`):

```
Prefetch: DataCopyIn(A1 Ping) + DataCopyIn(B1 Ping)                    <- before the K loop

k=0:  WaitFlag(A1Ping, B1Ping) -> DataLoad -> Compute -> DataCopyIn(B1Pong) -> DataCopyIn(A1Pong)
k=1:  DataLoad -> Compute
k=2:  DataLoad -> Compute
k=3:  DataLoad(release B1Ping) -> Compute -> DataCopyIn(load B1Ping)

k=4:  WaitFlag(B1Pong) -> DataLoad(B1Pong) -> Compute
k=5:  DataLoad -> Compute
k=6:  DataLoad -> Compute
k=7:  DataLoad(release A1Ping and B1Pong) -> Compute -> DataCopyIn(B1Pong) -> DataCopyIn(A1Ping)
k=8:  WaitFlag(A1Pong ready) -> WaitFlag(B1Ping ready) -> DataLoad -> Compute
...
```

### Performance Data Analysis

#### Atlas A2 Training Series Chip Performance Data

- Scenario 1: L2Cache splitting disabled, singleCoreM=2048, singleCoreN=1536, 24 cores full coverage in one round
- Scenario 2: L2Cache splitting enabled, singleCoreM=1024, singleCoreN=1536, 24 cores in 2 rounds

| Scenario | Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| Scenario 1 | 4121.16 | 24 | 3670.7 | 3081.664 | 0.84 | 337.343 | 0.092 | 2538.348 | 0.692 | 3552.248 | 0.968 | 160.405 | 0.044 |
| Scenario 2 | 4081.64 | 24 | 3636.85 | 3082.158 | 0.847 | 345.139 | 0.095 | 2553.064 | 0.702 | 3487.068 | 0.959 | 161.812 | 0.044 |

Excluding startup overhead, 84.7% of the chip's peak compute has been achieved.

After enabling L2Cache splitting, aic_mte2_time decreased from 3552.248μs to 3487.068μs, a 1.84% reduction. The current splitting strategy is relatively simple; users can further optimize L2Cache splitting strategy to improve MTE2 bandwidth.

#### Ascend 950PR Chip Performance Data

- Scenario 1: L2Cache splitting disabled, singleCoreM=2048, singleCoreN=1024, 32 cores full coverage in one round
- Scenario 2: L2Cache splitting enabled, singleCoreM=1024, singleCoreN=1024, 32 cores in 2 rounds

| Scenario | Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| Scenario 1 | 2572.047 | 32 | 2571.44 | 2564.813 | 0.997 | 144.604 | 0.056 | 828.001 | 0.322 | 1874.267 | 0.729 | 221.997 | 0.086 |
| Scenario 2 | 2574.492 | 32 | 2573.39 | 2564.147 | 0.996 | 104.845 | 0.041 | 819.207 | 0.318 | 1892.742 | 0.736 | 223.129 | 0.087 |

99.7% of the chip's peak compute has been achieved.

Enabling L2Cache splitting shows no significant effect on Ascend 950PR chip. The reasons are: L2Cache optimization targets MTE2 bound mitigation, but the current bottleneck is Cube computation rather than data transfer, so reducing MTE2 duration cannot improve overall performance; additionally, L2Cache splitting divides computation into 2 scheduling rounds, introducing extra Scalar and scheduling overhead, causing Scenario 2 Task Duration to be slightly higher than Scenario 1. Meanwhile, Scenario 2 aic_mte2_time (1892.742μs) is actually slightly higher than Scenario 1 (1874.267μs), because when the example is Cube bound, the MTE2 pipeline is blocked by Cube computation, and the profiler-collected aic_mte2_time includes pipeline wait time rather than pure data transfer time. Although L2Cache optimization reduces actual data access latency, it is masked by the Cube computation bottleneck and cannot be reflected in MTE2 metrics.

### Theoretical Performance Analysis

#### Cube Computation Performance Analysis

**Atlas A2 Training Series Chip**: Example parameters M=N=K=8192, baseM=128, baseN=256, baseK=64, clock frequency 1.85GHz, processing 16×16×16 multiply-accumulate operations per cycle.

$$cube\_time = \frac{M \times N \times K}{16 \times 16 \times 16 \times core\_num \times cube\_freq} = \frac{8192 \times 8192 \times 8192}{16 \times 16 \times 16 \times 24 \times 1850} = 3022.92\mu s$$

Cube computation duration error:

$$Error = \frac{aic\_mac\_time - cube\_time}{cube\_time} = \frac{3082.158 - 3022.92}{3022.92} = 1.96\%$$

**Ascend 950PR Chip**: Example parameters M=N=K=8192, baseM=256, baseN=256, baseK=64, clock frequency 1.65GHz, processing 16×16×16 multiply-accumulate operations per cycle.

$$cube\_time = \frac{M \times N \times K}{16 \times 16 \times 16 \times core\_num \times cube\_freq} = \frac{8192 \times 8192 \times 8192}{16 \times 16 \times 16 \times 32 \times 1650} = 2542.00\mu s$$

Cube computation duration error:

$$Error = \frac{aic\_mac\_time - cube\_time}{cube\_time} = \frac{2564.813 - 2542.00}{2542.00} = 0.90\%$$

#### MTE2 Bandwidth Analysis

**Total Read Data Volume**:

Atlas A2 Training Series Chip (baseM=128, baseN=256):

$$Total Read Data = \left(\frac{N}{baseN} \times M \times K + \frac{M}{baseM} \times K \times N\right) \times sizeof(half) = (32 \times 8192 \times 8192 + 64 \times 8192 \times 8192) \times 2B = 12GB$$

Ascend 950PR Chip (baseM=256, baseN=256):

$$Total Read Data = \left(\frac{N}{baseN} \times M \times K + \frac{M}{baseM} \times K \times N\right) \times sizeof(half) = (32 \times 8192 \times 8192 + 32 \times 8192 \times 8192) \times 2B = 8GB$$

**Theoretical MTE2 Duration**:

Atlas A2 Training Series Chip: The peak L2 Cache bandwidth is approximately 5 TB/s, and the HBM bandwidth is approximately 1.8 TB/s. Data is first read from HBM and subsequently read from the L2 Cache.

$$Total\ Data\ First\ Read\ from\ HBM = M \times K \times sizeof(half) + K \times N \times sizeof(half) = 256MB$$

$$Theoretical\ MTE2\ Duration = \frac{Data\ Read\ from\ HBM}{1.8TB/s} + \frac{Data\ Read\ from\ L2Cache}{5TB/s}$$

MTE2 duration error:

$$MTE2\ Duration\ Error = \frac{3487.068 - 2672.44}{2672.44} = 30.48\%$$

The current MTE2 duration differs significantly from the theoretical value because the actual chip has only 192 MB of L2 Cache and the current L2 Cache splitting strategy is relatively simple. In addition, when MTE2 performs an ND2NZ transfer, where GM data uses the ND layout and must be converted from ND to NZ while being transferred to L1, the L2 Cache bandwidth decreases. Users can further optimize the L2 Cache splitting strategy to improve MTE2 bandwidth.

Ascend 950PR Chip: The peak L2 Cache bandwidth is approximately 5 TB/s, and the HBM bandwidth is approximately 1.6 TB/s.

$$Theoretical\ MTE2\ Duration = \frac{Data\ Read\ from\ HBM}{1.6TB/s} + \frac{Data\ Read\ from\ L2Cache}{5TB/s}$$

MTE2 duration error:

$$MTE2\ Duration\ Error = \frac{1874.267 - 1832.10}{1832.10} = 2.30\%$$

Compared with the Atlas A2 Training Series Chip, the Ascend 950PR Chip transfers data more efficiently and achieves higher MTE2 bandwidth utilization.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Switch Case

  Specify the scenario to build through `-DSCENARIO_NUM=N` during cmake build. Scenario descriptions:
  - `1`: L2Cache splitting disabled
  - `2`: L2Cache splitting enabled

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
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project, default npu mode
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`, `dav-3510` | NPU hardware architecture: dav-2201 corresponds to A2/A3, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`, `2` | Scenario number: 1=L2Cache splitting disabled, 2=L2Cache splitting enabled |

- Execution results

  The following execution result indicates that the accuracy comparison succeeded.
  ```bash
  test pass!
  ```

## Function Debugging

### printf

This interface provides formatted output functionality in CPU domain or NPU domain debugging scenarios.

Call the printf interface in the operator kernel implementation code where log information needs to be output.

Example:

```cpp
AscendC::printf("matmul blockIdx=%d\n", AscendC::GetBlockIdx());
```

> **Notice** The printf (PRINTF) interface printing functionality will impact actual operator running performance and is typically used during the debugging phase. Developers can disable printing by setting ASCENDC\_DUMP=0 as needed.

### DumpTensor

For operators developed based on operator projects, this interface can be used to dump the content of specified Tensors. It also supports printing custom additional information (only uint32\_t data type information is supported), such as printing the current line number.

Call the DumpTensor interface in the operator kernel implementation code where Tensor data needs to be printed. Example:

```cpp
AscendC::DumpTensor(cLocal, baseM * baseN);
```

> **Notice** The DumpTensor interface printing functionality will impact actual operator running performance and is typically used during the debugging phase. Developers can disable printing by setting ASCENDC\_DUMP=0 as needed.

## Performance Debugging

### Introduction to the msOpProf Tool

msOpProf is a single-operator performance analysis tool with two usage modes: `msopprof` and `msopprof simulator`. It helps users identify anomalies in operator memory, code, and instructions for comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and file types (executables or operator binary `.o` files).

- On-device performance collection

    On-device performance collection directly measures the operator's execution time on the Ascend AI Processor. This method is suitable for quickly locating operator performance issues in an on-device environment.

    Run msopprof on the `demo` executable for operator tuning:
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

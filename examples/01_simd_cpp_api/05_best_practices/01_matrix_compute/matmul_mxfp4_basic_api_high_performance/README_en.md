# MxFP4 Matmul Basic API High-Performance Example

## Overview

This example introduces how to implement a high-performance MxFP4 Matmul kernel based on Ascend C basic API and static Tensor programming, through multiple optimization methods including L1/L0 double buffering, large packet transfer, and fine-grained pipeline synchronization.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```text
├── matmul_mxfp4_basic_api_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Ground truth comparison script
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── figures                 // Illustrations
│   ├── mmad_mx.asc             // Ascend C basic API example implementation
│   └── README.md               // Example documentation
```

## MxMatmul Introduction

Standard Matmul has only two matrix inputs:

$$
C = A * B
$$

MxMatmul additionally introduces two scale inputs on top of this. A and B are MxFP4 low-bit data, and scaleA and scaleB are scaling factors. During computation, matrix data and corresponding scales participate together in the operation:

$$
C = (\text{scaleA} \otimes A) \times (\text{scaleB} \otimes B)
$$

In MxMatmul, every 32 elements along the K direction share one scale. That is, the number of scales is much fewer than matrix elements, but it broadcasts along the K direction to the corresponding group of MxFP4 data, as shown in the figure:

  <img src="figures/MxMatmul.png"  width="80%">

## Example Specifications

This example implements a fixed shape `8192 x 8192 x 8192` MxFP4 Matmul, with output data type `bfloat16_t`.

### Input/Output

| Input/Output | Logical Shape | Data Type | Data Layout Type | Description |
|------|------|----------|------|------|
| A | `[M, K]` | `fp4x2_e1m2_t` | ND | Left matrix, 2 fp4 elements packed per byte |
| scaleA | `[M, scaleK]` | `fp8_e8m0_t` | ND | Scaling factor for A matrix, every 32 elements along K direction share one scale |
| B | `[N, K]` | `fp4x2_e1m2_t` | ND | Right matrix, input to kernel in `[N, K]` form |
| scaleB | `[N, scaleK]` | `fp8_e8m0_t` | ND | Scaling factor for B matrix, every 32 elements along K direction share one scale |
| C | `[M, N]` | `bfloat16_t` | ND | Output matrix |

Where:
- Two `fp4x2_e1m2_t` are packed and stored in one byte, so when data type is `fp4`, K must be even
- `scaleK = align_even(ceil(K / 32))`, meaning first ceil then align to a multiple of 2. Due to hardware constraints requiring scale data to satisfy 2Byte continuous alignment along K direction, scaleK must be even
- Due to hardware constraints, when scaleB matrix is input as `[scaleK, N]`, K direction 2 Byte continuity is required, so ScaleB input as `[N, scaleK]` is recommended

#### Four-Way Input Description

Compared to standard Matmul, MxMatmul has additional scale transfer, loading, and computation paths in the kernel. A/B and scaleA/scaleB must enter the computation pipeline at the same K block pace, otherwise the Cube side cannot obtain matching scaling information.

- Four-way input data layout format is shown below:

  <img src="figures/formatOfMx.png">

- Four-way input transfer and computation is shown below:

  <img src="figures/InputOfMxMatmul.png">

### Key Parameters

| Parameter | Value | Description |
|------|----|------|
| `M` | `8192` | Matrix M dimension size |
| `N` | `8192` | Matrix N dimension size |
| `K` | `8192` | Matrix K dimension size |
| `baseM` | `256` | Cube computation basic block M dimension size |
| `baseK` | `256` | Cube computation basic block K dimension size |
| `baseN` | `256` | Cube computation basic block N dimension size |
| `singleCoreM` | `2048` | Single core M direction computation range |
| `singleCoreN` | `1024` | Single core N direction computation range |
| `singleCoreK` | `8192` | Single core K direction computation range |
| `stepKa` | `2` | GM->L1 transfer, A K direction large packet transfer step |
| `stepKb` | `2` | GM->L1 transfer, B K direction large packet transfer step |
| `scaleFactorKa` | `4` | GM->L1 transfer, scaleA relative to A K direction transfer ratio |
| `scaleFactorKb` | `4` | GM->L1 transfer, scaleB relative to B K direction transfer ratio |
| `Block Num` | `32` | Number of cores used |

> **Constraint Description**: This example currently does not support scenarios with K direction tail blocks, requiring `K` to be divisible by `baseK`.

## Example Implementation

The entire kernel process is: after multi-core splitting, each core is responsible for one singleCoreM * singleCoreN sub-matrix, looping in M/N directions to transfer A/B and scale into L1, then looping in K direction to load into L0A/L0B, completing Cube accumulation computation, and finally writing results back to GM.

### Data Flow Path

The overall data flow is:

```text
Physical Address                        Pipeline
  GM
  |  DataCopyInA / DataCopyInAScale   MTE2
  |  DataCopyInB / DataCopyInBScale   MTE2
  v
  L1
  |  DataLoadA(with MX scaleA)        MTE1
  |  DataLoadB(with MX scaleB)        MTE1
  v
  L0A / L0B / L0A_MX / L0B_MX
  |
  |  Mmad                              M
  v
  L0C
  |
  |  Fixpipe: float -> bfloat16       FIX
  v
  GM
```

#### 1. Multi-Core Splitting: Splitting the Large Matrix into 32 Sub-Tasks

The example uses 32 cores in parallel. `M=8192` is split into 4 parts by `singleCoreM=2048`, `N=8192` is split into 8 parts by `singleCoreN=1024`, forming exactly `4 x 8 = 32` sub-matrices.

```text
M direction: 8192 / 2048 = 4 blocks
N direction: 8192 / 1024 = 8 blocks

Total blocks = 4 * 8 = 32
```

Each core only processes its assigned `2048 x 1024` output region. Within the core, `baseK=256` blocks loop in the K direction to complete accumulation.

The corresponding core index computation is:

```cpp
constexpr uint32_t mIter = AscendC::Std::ceil_div(M, singleCoreM);
uint32_t mIterIdx = AscendC::GetBlockIdx() % mIter;
uint32_t nIterIdx = AscendC::GetBlockIdx() / mIter;

uint64_t gmOffsetA = mIterIdx * singleCoreM * K;
uint64_t gmOffsetB = nIterIdx * K * singleCoreN;
uint64_t gmOffsetC = mIterIdx * singleCoreM * N + nIterIdx * singleCoreN;
```

#### 2. GM to L1: Matrix and Scale Entering Pipeline Together

MxMatmul input is not two-way but four-way: A, B, scaleA, scaleB. Therefore GM to L1 transfer is also divided into four types:

| Data | Transfer Function | GM Source Data | L1 Target |
|------|----------|-----------|---------|
| A | `DataCopyInA` | `A [M, K]` | `A1` |
| B | `DataCopyInB` | `B [N, K]` | `B1` |
| scaleA | `DataCopyInAScale` | `scaleA [M, scaleK]` | `scaleA1` |
| scaleB | `DataCopyInBScale` | `scaleB [N, scaleK]` | `scaleB1` |

Through `stepKa=2`, `stepKb=2`, transfer `stepKa * baseM * baseK` A matrix data and `stepKb * baseN * baseK` B matrix data at once, reducing transfer instruction count and improving GM to L1 transfer efficiency.

Scale transfer granularity is controlled by `scaleFactorKa=4`, `scaleFactorKb=4`, transferring `scaleFactorKa * stepKa * baseM * baseSK` ScaleA matrix data and `scaleFactorKb * stepKb * baseN * baseSK` ScaleB matrix data at once, where `baseSK = baseK / 32`. Its purpose is to make scaleA/scaleB cover a larger range in K direction compared to A/B per transfer, reducing MTE2 pressure from repeated scale transfers.

Taking the A side as example, `DataCopyInA` uses `Nd2NzParams` to transfer `stepKa` K direction base blocks at once, each base block being `baseM * baseK`:

```cpp
constexpr uint32_t packedStepK = AscendC::Std::ceil_div(baseK * stepKa, 2);
AscendC::Nd2NzParams nd2nzA1Params;
nd2nzA1Params.ndNum = 1;
nd2nzA1Params.nValue = curM;
nd2nzA1Params.dValue = packedStepK;
nd2nzA1Params.srcDValue = PACKED_K;
nd2nzA1Params.dstNzC0Stride = baseM;
nd2nzA1Params.dstNzNStride = 1;
AscendC::DataCopy(a1Local, aGM[kChunkIdx * baseK + mBlockIdx * K * baseM], nd2nzA1Params);
```

Scale side transfers as b16. Since `Mmad` requires K direction continuity for minimum fractal data reading, correspondingly scale data must satisfy 2Byte continuity along K direction, so b16 type transfer is used to ensure correct data layout:

```cpp
constexpr uint32_t stepScaleK = AscendC::Std::ceil_div(baseK * stepKa * scaleFactorKa, SCALE_CEIL_NUMBER);
AscendC::Dn2NzParams dn2nzParams;
dn2nzParams.dValue = curM;
dn2nzParams.nValue = stepScaleK / 2;
dn2nzParams.srcDValue = SCALE_K / 2;
dn2nzParams.dstNzC0Stride = stepScaleK / 2;

auto asLocalB16 = as1Local.ReinterpretCast<half>();
AscendC::DataCopy(asLocalB16, asGMB16, dn2nzParams);
```

#### 3. L1 to L0: LoadData Entering MX Computation Path

Standard Matmul only needs to load A/B; MxMatmul `LoadData` also needs to bring the corresponding scale, transferring one base block at a time.

```text
A1 block + scaleA1 block -> L0A MX data
B1 block + scaleB1 block -> L0B MX data
```

After A/B and scale enter L0, the Cube computation unit can complete scaled matrix multiplication accumulation according to MX semantics.

The key code is `LoadData` receiving both matrix LocalTensor and scale LocalTensor simultaneously:

```cpp
uint32_t srcAddr = kOffsetInChunkA * baseK * baseM;
uint32_t scaleSrcAddr = (kOffsetInScaleChunkA * baseK / SCALE_CEIL_NUMBER) * CUBE_BLOCK;

AscendC::LoadData(a2Local,
                  a1Local[srcAddr],
                  as1Local[scaleSrcAddr],
                  loadDataParams,
                  loadMxDataParams);
```

#### 4. L0 Double Buffering: Overlapping Loading and Computation

The example uses Ping-Pong double buffering at both L1 and L0. The core goal is to overlap transfer, loading, and computation as much as possible to reduce waiting.

The main buffer layout of this example is:

| Level | Buffer | Content | Purpose |
|------|--------|------|------|
| L1 | `A1 Ping/Pong` | A large packet data | Stores GM->L1 A data |
| L1 | `B1 Ping/Pong` | B large packet data | Stores GM->L1 B data |
| L1 | `scaleA1 Ping/Pong` | Scale data corresponding to A | Stores A side MXScale |
| L1 | `scaleB1 Ping/Pong` | Scale data corresponding to B | Stores B side MXScale |
| L0A | `A2 Ping/Pong` | Current K block A data | Cube `Mmad` left operand |
| L0B | `B2 Ping/Pong` | Current K block B data | Cube `Mmad` right operand |
| L0C | `cLocal` | float accumulation result | `Mmad` output, for `Fixpipe` write-back |

> **Note**: `L0A_MX` and `L0B_MX` are used to store scale data. Their addresses have a fixed relationship with `L0A`/`L0B` and do not require manual allocation by users.

The corresponding `LocalTensor` objects are created as follows:

```cpp
AscendC::LocalTensor<fp4x2_e1m2_t> a1LocalPing(AscendC::TPosition::A1, 0, a1BufSize);
AscendC::LocalTensor<fp4x2_e1m2_t> a1LocalPong(AscendC::TPosition::A1, a1BufSize, a1BufSize);
AscendC::LocalTensor<fp8_e8m0_t> as1LocalPing(AscendC::TPosition::A1, 2 * a1BufSize, as1BufSize);
AscendC::LocalTensor<fp8_e8m0_t> as1LocalPong(AscendC::TPosition::A1, 2 * a1BufSize + as1BufSize, as1BufSize);

AscendC::LocalTensor<fp4x2_e1m2_t> a2LocalPing(AscendC::TPosition::A2, 0, a2PingpongSize);
AscendC::LocalTensor<fp4x2_e1m2_t> a2LocalPong(AscendC::TPosition::A2, L0_PINGPONG_BYTES, a2PingpongSize);
```

A simplified pipeline rhythm is:

```text
time     |---------------------------------------------------------------------------->

GM->L1   | A/B/scaleA/scaleB Ping | A/B/scaleA/scaleB Pong | A/B/scaleA/scaleB Ping |
L1->L0                            | L0 Ping load --|       | L0 Pong load --|
Cube                                               | Mmad Ping ---|   | Mmad Pong ---|
Fixpipe                                                           | fixpipe C ---
```

The horizontal line lengths in the figure only express that different stages may have different durations and do not represent measured ratios; actual durations require `msopprof` collection results.

In the K loop, when the current K block enters Cube computation, the next batch of A/B/scaleA/scaleB can initiate transfer in advance. Ping and Pong alternate, with producers and consumers confirming buffer writability and data readability through event synchronization.

#### 5. Cube Computation: Accumulation by K Block

Each output sub-matrix loops in the K direction at the innermost level. Each round processes one `baseK=256` K block, then loops in M/N directions:

```text
for nBlock in N blocks:
  for mBlock in M blocks:
    for kBlock in K blocks:
        LoadData(A block, scaleA block)
        LoadData(B block, scaleB block)
        Mmad accumulate
```

`Mmad` input comes from L0A/L0B, output accumulates to L0C. The first K block initializes the accumulation result, subsequent K blocks continue accumulating until the complete K direction computation is finished.

Key parameters in the computation stage include `m/n/k` dimensions and whether to initialize the C matrix:

```cpp
AscendC::MmadParams mmadParams;
mmadParams.m = curM;
mmadParams.n = curN;
mmadParams.k = baseK;
mmadParams.cmatrixInitVal = (kBlockIdx == 0);
AscendC::Mmad(cLocal, a2Local, b2Local, mmadParams);
```

Where `cmatrixInitVal` controls the first K block to initialize the accumulation result, and subsequent K blocks continue accumulating on existing L0C data.

#### 6. Fixpipe Write-Back: Converting from float to bfloat16 Output

Cube side accumulation results are saved in L0C with float data type. After computation completes, `Fixpipe` converts results to `bfloat16_t` and writes back to GM.

```text
L0C float result
      |
      | Fixpipe, F32 -> BF16
      v
GM C [M, N]
```

Write-back completes F32 to BF16 conversion through `quantPre`:

```cpp
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
fixpipeParams.nSize = curN;
fixpipeParams.mSize = curM;
fixpipeParams.srcStride = curMAlign;
fixpipeParams.dstStride = N;
fixpipeParams.quantPre = QuantMode_t::F322BF16;
AscendC::Fixpipe(cGM[mBlockIdx * baseM * N + nBlockIdx * baseN], cLocal, fixpipeParams);
```

### Event Synchronization: Pipelining the Flow

When using static Tensor programming, synchronization is key to performance and correctness. This example primarily uses four types of events, with eventID controlled from `EVENT_ID0` to `EVENT_ID3`:

| Event | Direction | Purpose | Flag Number |
|------|------|------|----------|
| `MTE2_MTE1` | GM->L1 notifies L1->L0 | After DataCopyIn completes, notifies DataLoad that L1 data can be read | `EVENT_ID0/1`: A+B Data Ping/Pong; `EVENT_ID2/3`: As+Bs Scale Ping/Pong |
| `MTE1_MTE2` | L1->L0 notifies GM->L1 | After DataLoad consumes L1 data, notifies DataCopyIn that the buffer can be overwritten | Same as above |
| `MTE1_M` | L1->L0 notifies Cube | After LoadData completes, notifies Mmad that computation can start | `EVENT_ID0/1`: L0 Ping/Pong |
| `M_MTE1` | Cube notifies L1->L0 | After Mmad consumes L0 buffer, notifies next LoadData that it can write | `EVENT_ID0/1`: L0 Ping/Pong |

A/B data have the same lifecycle, so they are bound to the same event group by Ping/Pong phase; scaleA/scaleB have the same lifecycle, but scale chunks are typically larger than data chunks, so they use a separate Ping/Pong event group. The code ensures through compile-time constraints that `stepKa == stepKb` and `stepKa * scaleFactorKa == stepKb * scaleFactorKb`.

Reverse synchronization needs to be preset before entering the main loop, otherwise the first wait for a writable buffer will block:

```cpp
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID0);
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID1);
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID2);
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID3);
AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID0);
AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID1);
```

Large packet granularity synchronization only waits for data readiness at the first element of the large packet and releases the buffer at the last element:

```cpp
if (kOffsetInDataChunk == 0) {
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(GetDataEventId(dataReadIdx));
}
if (((kOffsetInDataChunk + 1) == dataChunkStep) || (kBlockIdx + 1 == kLoopCount)) {
    AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(GetDataEventId(dataReadIdx));
}
if (kOffsetInScaleChunk == 0) {
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(GetScaleEventId(scaleReadIdx));
}
if (((kOffsetInScaleChunk + 1) == scaleChunkStep) || (kBlockIdx + 1 == kLoopCount)) {
    AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(GetScaleEventId(scaleReadIdx));
}
```

From the pipeline perspective, these events connect three stages:

```text
DataCopyIn(GM->L1)
        |
        | MTE2_MTE1
        v
LoadData(L1->L0)
        |
        | MTE1_M
        v
Mmad(Cube)
        |
        | M_MTE1
        v
next LoadData
```

Reverse events are also important. For example, `MTE1_MTE2` indicates that a Ping/Pong buffer in L1 has been consumed and can be overwritten by the next GM->L1 transfer. Without these reverse synchronizations, the pipeline can easily encounter issues of overwriting unconsumed data or waiting for incomplete data.

## Performance Optimization Summary

The performance optimization of this example revolves around three things:

- **Multi-core splitting**: Split the `8192 x 8192` output matrix across 32 cores for parallel computation.
- **Large packet transfer**: A/B transfer by multiple K blocks, scale transfers with larger K coverage range, reducing MTE2 instruction and data transfer pressure.
- **Double buffer pipeline**: Both L1 and L0 use Ping-Pong, allowing DataCopyIn, LoadData, Mmad, and Fixpipe to overlap execution as much as possible.

## Theoretical Performance Comparison

Performance metric description table:
| Metric | Description |
|------|------|
| `Task Duration(μs)` | Total execution time of the entire task, operator end-to-end execution time is determined by this parameter |
| `Block Num` | Number of cores used, that is, the number of blocks launched by the kernel |
| `aicore_time(μs)` | Average AI Core execution time |
| `aic_mac_time(μs)` | Cube computation unit execution time, primarily corresponding to `Mmad` stage |
| `aic_mac_ratio` | Cube computation unit time ratio, reflecting computation unit utilization |
| `aic_scalar_time(μs)` | Scalar instruction execution time, reflecting loop scheduling, address computation, parameter configuration overhead |
| `aic_scalar_ratio` | Scalar time ratio |
| `aic_mte1_time(μs)` | MTE1 execution time, primarily corresponding to L1 to L0A/L0B `LoadData` |
| `aic_mte1_ratio` | MTE1 time ratio, reflecting L1 to L0 data transfer pressure |
| `aic_mte2_time(μs)` | MTE2 execution time, primarily corresponding to GM to L1 `DataCopyIn` |
| `aic_mte2_ratio` | MTE2 time ratio, reflecting GM to L1 data loading pressure |
| `aic_fixpipe_time(μs)` | Fixpipe execution time, primarily corresponding to L0C to GM result write-back |
| `aic_fixpipe_ratio` | Fixpipe time ratio, reflecting result write-back memory access pressure |

Ascend 950PR chip performance data:

| Case version | Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| Basic API MxMatmul | 681.056 | 32 | 679.99 | 640.16 | 0.941 | 62.824 | 0.092 | 312.728 | 0.46 | 588.736 | 0.866 | 32.422 | 0.048 |

Cube computation accounts for `94.1%` of the AI Core time in this example (that is, `aic_mac_ratio` in the table).

### Cube Computation Performance Analysis

The performance data of this example was obtained running on Ascend 950PR, which has a clock frequency of 1.65GHz. For MX-FP4 data type, it processes 16×64×16 multiply-accumulate operations per cycle.

Cube theoretical computation time $T_{cube}$ is:

$$T_{cube} = \frac{M \times N \times K}{16 \times 64 \times 16 \times 1.65 \times 10^9 \times \text{core count}} = \frac{8192 \times 8192 \times 8192}{16384 \times 1.65 \times 10^9 \times 32} = 635.5 μs$$

From the table, `aic_mac_time` is `640.16 μs`, relative to theoretical value `635.5 μs`, error $E_{cube}$ is:

$$E_{cube} = \frac{T_{actual} - T_{cube}}{T_{cube}} = \frac{640.16 - 635.5}{635.5} = 0.73 \%$$

### MTE2 Bandwidth Analysis

**Data Reuse Principle**:

In matrix multiplication, each element $C_{i,j}$ of the output matrix C requires row i of A and column j of B to participate in computation. During block computation, the same input data block is reused by multiple output blocks:

- A matrix is split into `M/baseM` row blocks along M direction, each A row block participates in `N/baseN` output blocks along N direction
- B matrix is split into `N/baseN` column blocks along N direction, each B column block participates in `M/baseM` output blocks along M direction

MxFP4 Matmul also includes two scale inputs, with every 32 elements along K direction sharing one scale, so `scaleK = K/32`:

- scaleA shape `[M, scaleK]`, split into `M/baseM` row blocks along M direction, each scaleA row block participates in `N/baseN` output blocks along N direction
- scaleB shape `[N, scaleK]`, split into `N/baseN` column blocks along N direction, each scaleB column block participates in `M/baseM` output blocks along M direction

Due to limited L1/L2Cache capacity, not all input data can be cached, and the same data block is transferred from HBM to L2Cache/L1 multiple times, causing repeated data transfers.

**Total Read Data Volume**:

MxFP4 Matmul input includes four-way data: A, B, scaleA, scaleB. A/B matrices use `fp4x2_e1m2_t`, with every 2 fp4 elements packed as 1 byte, single element `sizeof = 0.5B`; scaleA/scaleB use `fp8_e8m0_t`, single element `sizeof = 1B`.

This example parameters `M=N=K=8192`, `scaleK=256`, block parameters `baseM=baseN=256`.

Total read data volume $D_{total}$ is:

$$D_{total} = \frac{N}{baseN} \times M \times K \times 0.5B + \frac{M}{baseM} \times K \times N \times 0.5B + \frac{N}{baseN} \times M \times scaleK \times 1B + \frac{M}{baseM} \times scaleK \times N \times 1B$$

$$= (32 \times 8192 \times 8192 \times 0.5 + 32 \times 8192 \times 8192 \times 0.5 + 32 \times 8192 \times 256 \times 1 + 32 \times 256 \times 8192 \times 1) B$$

$$= (1GB + 1GB + 64MB + 64MB) = 2.125GB$$

**MTE2 Theoretical Duration**:

Ascend 950PR chip: L2Cache peak bandwidth approximately 5TB/s, HBM (corresponding to GM) bandwidth approximately 1.6TB/s. In the most ideal case, first access fetches data from HBM and caches to L2Cache, subsequent accesses read directly from L2Cache.

> **Unit Description**: Bandwidth units use decimal, 1 TB/s = 10^12 B/s.

First HBM read data volume $D_{HBM}$ is:

$$D_{HBM} = M \times K \times 0.5B + K \times N \times 0.5B + M \times scaleK \times 1B + N \times scaleK \times 1B = 32MB + 32MB + 2MB + 2MB = 68MB$$

L2Cache read data volume $D_{L2Cache}$ is:

$$D_{L2Cache} = D_{total} - D_{HBM} = 2.125GB - 68MB \approx 2.057GB$$

MTE2 theoretical duration $T_{MTE2}$ is:

$$T_{MTE2} = \frac{D_{HBM}}{1.6TB/s} + \frac{D_{L2Cache}}{5TB/s} = \frac{68MB}{1.6TB/s} + \frac{2.057GB}{5TB/s} \approx 42.5μs + 411.4μs = 453.9μs$$

MTE2 duration error $E_{MTE2}$ is:

$$E_{MTE2} = \frac{T_{actual} - T_{MTE2}}{T_{MTE2}} = \frac{588.736μs - 453.9μs}{453.9μs} = 29.7\%$$

Ascend 950PR L2Cache size is 128MB and cannot cache all input data. Some data experiences L2Cache miss during transfer and needs to be fetched from HBM. Users can further optimize L2Cache splitting strategy to improve MTE2 bandwidth.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

> **Notice**: This example requires `ml_dtypes` library version `0.2.0` and `en_dtypes` library version `0.0.4`. Installation command:

```bash
python3 -m pip install ml_dtypes==0.2.0 en_dtypes==0.0.4
```

### Configure Environment Variables

Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.


```bash
source ${install_path}/cann/set_env.sh
```

> **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

### Run the Example

Run the following commands in this example directory.

```bash
mkdir -p build && cd build;  # Create and enter the build directory
cmake .. -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j;
python3 ../scripts/gen_data.py
./demo
python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin
```

To use NPU simulation mode, set `-DCMAKE_ASC_RUN_MODE=sim`.

```bash
cmake .. -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j; # NPU mode
cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j; # NPU simulation mode
```

Build option description:

| Parameter | Values | Description |
|------|--------|------|
| `CMAKE_ASC_RUN_MODE` | `npu` / `sim` | Run mode: NPU execution, NPU simulation |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | Target SoC architecture |

> **Notice:** Clear CMake cache before switching `CMAKE_ASC_RUN_MODE` / `CMAKE_ASC_ARCHITECTURES`. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

The following execution result indicates that the accuracy comparison succeeded.

```bash
test pass!
```

### Performance Analysis

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

View the detailed performance analysis results:

```bash
cat ./OPPROF_*/PipeUtilization.csv
```

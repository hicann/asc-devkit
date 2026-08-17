# Add Performance Tuning Example

## Overview

This example uses addition to introduce performance tuning methods based on static Tensor programming. The entire tuning process is divided into seven steps (case 0-6), progressively demonstrating the complete tuning path from scalar operations to vector operations, from single-core to multi-core, and from basic implementation to deep optimization.

**Optimization Path**:
- Case 0: Single-core scalar version (baseline)
- Case 1: Single-core vector version
- Case 2: Multi-core even splitting + small block transfer
- Case 3: Multi-core even splitting + large block transfer
- Case 4: Multi-core even splitting + double buffer optimization
- Case 5: Multi-core even splitting + double buffer + L2Cache bypass
- Case 6: Multi-core even splitting + double buffer + L2Cache bypass + Bank Conflict avoidance

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── add_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Ground truth comparison script
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── figures                 // Illustrations
│   ├── add.asc                 // Ascend C example implementation (including 7 optimization cases)
│   └── README.md               // Example documentation
```

## Example Description

**Example Function**:

The example implements addition of two matrices with fixed shape 8192×8192.

The Add computation formula is:

$$
  z = x + y
$$

  - x: Input, shape [8192, 8192], data type half;
  - y: Input, shape [8192, 8192], data type half;
  - z: Output, shape [8192, 8192], data type half;

## Example Implementation

### Performance Metric Description

**Table 1 AI Core Performance Metric Field Description**
| Field Name | Field Meaning |
|:---:|:---|
| Task Duration(μs)|Total Task duration, including scheduling time to the accelerator, execution time on the accelerator, and completion response time.|
| aiv_time|Task execution time on AI Vector Core, in μs.|
| aiv_vec_time(μs) | vec type instruction (vector computation instructions) duration, in μs. |
| aiv_vec_ratio | Ratio of vec type instruction cycle count to total cycle count. |
| aiv_scalar_time(μs) | scalar type instruction (scalar computation instructions) duration, in μs. |
| aiv_scalar_ratio | Ratio of scalar type instruction cycle count to total cycle count. |
| aiv_mte2_time(μs) | mte2 type instruction (GM->UB transfer instructions) duration, in μs. |
| aiv_mte2_ratio | Ratio of mte2 type instruction cycle count to total cycle count. |
| aiv_mte3_time(μs) | mte3 type instruction (UB->GM transfer instructions) duration, in μs. |
| aiv_mte3_ratio | Ratio of mte3 type instruction cycle count to total cycle count. |

The performance data in this chapter was obtained running on Atlas A2 Training Series Products.

### Case 0: Single-Core Scalar Version (Baseline Program)

**Implementation**: Refer to `KernelAdd::ProcessScalar()` function implementation

The baseline program implements addition of two sets of `half` type input data, using `for` loop `scalar` operations for computation.

**Key Code**:
```cpp
for (uint32_t i = 0; i < curLen; i++) {
      float xVal = (float)xLocal.GetValue(i);
      float yVal = (float)yLocal.GetValue(i);
      zLocal.SetValue(i, (half)(xVal + yVal));
    }
```

**Example Configuration**:
- Single-core scalar computation
- `dataCopyLen = 4096` is the number of data elements per transfer
- Single transfer data volume is 4096 * 2B = 8192 Byte, single scalar processed data volume is 4 Byte

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 1239689.1 | 1239688.63 | 0.015 | 0 | 1233742.494 | 0.995 | 5916.341 | 0.005 | 2485.465 | 0.002 |

**Optimization Effect Analysis**:
- End-to-end duration: **1239689.1μs** (approximately 1.24 seconds)
- Scalar instruction duration: 1233742.494μs, ratio **99.5%**
- Vector instruction duration: 0.015μs, ratio close to 0%
- Performance bottleneck: Scalar operations execute serially and cannot utilize hardware parallelism. This scenario is only used as an Add operation performance comparison example. In actual business scenarios, Scalar computation is not recommended.

**Principle Description**:
- Scalar operations can only process 1 data element at a time, requiring element-by-element looping
- AI Core hardware advantages lie in vector/matrix parallel computation; scalar operations cannot leverage hardware capabilities

**Performance Optimization Recommendation**:
> **Avoid scalar loops, use vector instructions**
> 
> In Ascend C programming, avoid using `for` loops with `GetValue/SetValue` scalar operations. Using vector instructions such as `AscendC::Add` can bring orders of magnitude performance improvement.

---

## Intermediate Performance Optimization

### Case 1: Single-Core Vector Version

**Implementation**: Refer to `KernelAdd::ProcessSingle()` function implementation

Convert scalar operations to vector operations, using `AscendC::Add` vector instructions to replace scalar loops, significantly improving computation efficiency.

**Key Code**:
```cpp
AscendC::Add(zLocal, xLocal, yLocal, curLen);
```

**Example Configuration**:
- Single-core computation
- `dataCopyLen = 4096` is the number of data elements per transfer
- Single `DataCopy` transfer data volume is 8192 Byte
- Single `Add` processes two input `Tensor`s, total processed data volume is 16384 Byte


**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 6909.6 | 6909.14 | 761.65 | 0.11 | 231.166 | 0.033 | 6208.762 | 0.873 | 2613.205 | 0.378 |

**Optimization Effect Analysis**:
- End-to-end performance: 6909.6μs, improved by **99.4%** compared to Case 0
- Scalar instruction duration: Decreased from 1233742.494μs to 231.166μs, significantly reducing scalar instructions
- Vector instruction duration: 761.65μs, ratio 11%
- Data transfer duration: 6208.762μs, ratio 87.3%, transfer pipeline serial improvement

**Principle Description**:
- Vector instructions can process multiple data elements at once (in this example, 4096*2 half elements per operation)
- Vector unit parallel computation capability far exceeds scalar unit
- But data transfer becomes the bottleneck, indicating computation speed is already faster than data supply. In single-core mode, data transfer request volume is insufficient, causing bandwidth to not be fully utilized

**Performance Optimization Recommendation**:
> **Use vector instructions to replace scalar loops**
> 
> Using vector APIs such as `AscendC::Add`, `AscendC::Mul` to replace element-by-element scalar loops can fully utilize AI Core vector computation units, achieving over 100x performance improvement.

> **Single-core usage is not recommended**

**Next Optimization Direction**:
- Data transfer (MTE2) accounts for 87.3%, becoming the main bottleneck
- Need to improve bandwidth utilization through multi-core parallelism and increased transfer granularity

---

### Case 2: Multi-Core Even Splitting + Small Block Transfer

**Implementation**: Refer to `KernelAdd::Process()` function implementation

Enable multi-core parallel computation, splitting the 8192×8192 matrix across multiple AIV Cores for parallel processing, using an even splitting strategy.

**Example Configuration**:
- Row direction split into 48 parts, evenly distributing data across 48 cores
- `dataCopyLen = 4096` is the number of data elements per transfer
- Single `DataCopy` transfer data volume is 4096 * 2B = 8192 Byte
- Single `Add` processes two input `Tensor`s, total processed data volume is 16384 Byte


**Key Code**:
```cpp
// Even splitting computation of rows processed per core (M direction)
uint32_t baseCoreM = totalM / splitM;
uint32_t remainderM = totalM % splitM;
if (blockIdxM < remainderM) {
    actualCoreM = baseCoreM + 1;  // Evenly distribute remainder
    startM = blockIdxM * actualCoreM;
} else {
    actualCoreM = baseCoreM;
    startM = remainderM * (baseCoreM + 1) + (blockIdxM - remainderM) * baseCoreM;
}
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 306.58 | 297.14 | 15.885 | 0.053 | 8.219 | 0.028 | 215.485 | 0.725 | 54.33 | 0.183 |

**Optimization Effect Analysis**:
- End-to-end task duration is 306.58μs, reduced by **95.5%** compared to Case 1
- Data transfer MTE2 duration is 215.485μs, ratio 72.5%

**Principle Description**:
- 48 AIV Cores process in parallel, theoretically achieving 48x speedup
- Reasons actual speedup is below theoretical:
  - Data transfer remains the bottleneck (mte2 ratio 72.5%)
- Even splitting ensures balanced load across cores, avoiding load imbalance where some cores are idle while others are busy

- This example chooses to split `M` direction into 48 parts (`splitM=48, splitN=1`) instead of splitting `M` into 8 and `N` into 6. The core purpose is to keep data processed per core continuous on GM, enabling single `DataCopy` to transfer `dataCopyLen` in the block loop

  <img src="figures/SplitCoreM.png" width="50%">

**Performance Optimization Recommendation**:
> **Fully utilize multi-core parallelism with even splitting strategy**
> 
> 1. Evenly distribute data across multiple AIV Cores for parallel computation
> 2. Use even splitting strategy (remainder distributed to first few cores) to ensure load balance
> 3. Splitting granularity needs to consider: core count, data volume, UB space size

**Next Optimization Direction**:
- MTE2 accounts for 72.5%, MTE3 accounts for 18.3%, transfer remains the bottleneck
- Computation only accounts for 5.3%, indicating "computation is fast, transfer is slow"
- Can improve bandwidth utilization by increasing single transfer data volume
---

### Case 3: Multi-Core Even Splitting + Large Block Data Transfer

**Implementation**: Refer to `KernelAdd::Process()` function implementation

To fully utilize bandwidth resources, increase the data volume of transfer instructions.

**Example Configuration**:
- Row direction split into 48 parts, evenly distributing data across 48 cores
- `dataCopyLen = 16384` is the number of data elements per split (4x Case 2)
- Single `DataCopy` transfer data volume is 32768 Byte
- Single `Add` processes two input `Tensor`s, total processed data volume is 65536 Byte


**Performance Data**:

| Task Duration(μs)  | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 268.5 |261.5 | 12.845 | 0.049 | 2.833 | 0.011 | 184.369 | 0.705 | 57.354 | 0.219 |

**Optimization Effect Analysis**:
- End-to-end performance: 268.5μs, reduced by **12.5%** compared to Case 2
- By increasing single transfer data volume, MTE2 duration decreased from 215.485μs to 184.369μs, reduced by **14.4%**

**Principle Description**:
- Increasing single transfer data volume reduces transfer count
- Larger continuous data blocks better utilize memory bandwidth
- But data volume is limited by UB space size (in this example, UB needs to hold x, y, z three data portions)

**Performance Optimization Recommendation**:
> **Increase single data transfer volume, reduce transfer count**
> 
> 1. Within UB space limits, maximize `dataCopyLen`
> 2. Use continuous large block data transfer, avoiding frequent small data block transfers
> 3. Need to balance UB space usage and transfer efficiency


> **Notice: dataCopyLen is not the bigger the better**
> 
> Building on Case 3, if dataCopyLen is further increased (for example, from 16384 to 16512), end-to-end performance is basically flat (268.5μs vs 267.76μs). It is recommended to consider data total volume, UB space, and alignment requirements comprehensively to determine the optimal dataCopyLen value.

---

### Case 4: Double Buffer Optimization

**Implementation**: Refer to `KernelAdd::ProcessDoubleBuffer()` function implementation

Adopt Double Buffer technology to implement pipeline parallelism of data transfer and computation, hiding memory access latency.

**Key Code**:
```cpp
// Ping-Pong double buffer addresses
static constexpr uint32_t xAddrPing = 0;
static constexpr uint32_t yAddrPing = MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t zAddrPing = yAddrPing + MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t xAddrPong = zAddrPing + MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t yAddrPong = xAddrPong + MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t zAddrPong = yAddrPong + MAX_DATA_COPY_LEN * sizeof(half);

// Double buffer pipeline: Alternate between two event IDs and two buffer groups
for (uint32_t loopIdx = 0; loopIdx < totalBlocks; loopIdx++) {
    int32_t eventID = (loopIdx % 2 == 0 ? EVENT_ID0 : EVENT_ID1);
    AscendC::LocalTensor<half> &xLocal = (loopIdx % 2 == 0 ? xPing : xPong);
    // ... Data transfer and computation, using corresponding eventID for synchronization
    AscendC::Add(zLocal, xLocal, yLocal, curLen);
}
```

**Example Configuration**:
- Row direction split into 48 parts, evenly distributing data across 48 cores
- `dataCopyLen = 16384` is the number of data elements per split
- Single `DataCopy` transfer data volume is 32768 Byte
- Single `Add` processes two input `Tensor`s, total processed data volume is 65536 Byte
- Split the data to be processed in half, enabling data transfer and Vector computation to execute in parallel

**Memory Layout**:

```
UB Memory Allocation (Double Buffer):
┌──────────────┐
│  xPing       │  0x00000
│  16384*2B    │
├──────────────┤
│  yPing       │  0x08000 (32768)
│  16384*2B    │
├──────────────┤
│  zPing       │  0x10000 (65536)
│  16384*2B    │
├──────────────┤
│  xPong       │  0x18000 (98304)
│  16384*2B    │
├──────────────┤
│  yPong       │  0x20000 (131072)
│  16384*2B    │
├──────────────┤
│  zPong       │  0x28000 (163840)
│  16384*2B    │
└──────────────┘
```

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 264.02 | 257.56 | 12.846 | 0.05 | 2.796 | 0.011 | 250.528 | 0.973 | 84.988 | 0.33 |

**Optimization Effect Analysis**:
- End-to-end performance: 264.02μs, reduced by **1.7%** compared to Case 3
- MTE2 duration increased from 184.369μs to 250.528μs (+35.9%), MTE3 duration increased from 57.354μs to 84.988μs (+48.2%). At this point, serial pure read bandwidth becomes mixed read-write bandwidth, so duration increases. Users should focus more on end-to-end duration reduction
- Due to double buffer being enabled, transfer and computation execute in parallel during the pipeline, hiding data transfer time and reducing Vector instruction wait time

**Principle Description**:
- **Ping-Pong Mechanism**:
  - While Ping buffer is computing, Pong buffer performs data transfer
  - Alternating execution achieves pipeline parallelism of computation and transfer, as shown below  
    <img src="figures/DoubleBuffer.png" width="50%">

**Performance Optimization Recommendation**:
> **Use double buffer to achieve transfer and computation parallelism**
> 
> 1. Double buffer benefits are greatest when computation and transfer times are similar
> 2. Requires sufficient UB space (approximately 2x single buffer space)
> 3. Use independent Event IDs to manage synchronization for the two buffer groups

**Next Optimization Direction**:
- Double buffer benefits are limited, indicating the bottleneck is in transfer speed itself
- Can try L2 Cache optimization to improve transfer efficiency
---

## Advanced Performance Refinement

### Case 5: Double Buffer + L2 Cache Bypass

**Implementation**: Refer to `KernelAdd::ProcessDoubleBufferL2Bypass()` function implementation (within this function, first set `SetL2CacheHint(CACHE_MODE_DISABLE)`, then call `ProcessDoubleBuffer()`)

Building on double buffer, for data that only needs to be loaded once, L2 Cache bypass can be set to load directly from GM to UB.

**Key Code**:
```cpp
  xGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
  yGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
```

**Example Configuration**:
- Row direction split into 48 parts, evenly distributing data across 48 cores
- `dataCopyLen = 16384` is the number of data elements per split
- Single `DataCopy` transfer data volume is 32768 Byte
- Single `Add` processes two input `Tensor`s, total processed data volume is 65536 Byte
- Split the data to be processed in half, enabling data transfer and Vector computation to execute in parallel

**L2 Cache Strategy**:
- xGm: Disable L2 Cache (one-time read)
- yGm: Disable L2 Cache (one-time read)

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 187.68 | 185.15 | 12.846 | 0.069 | 4.479 | 0.024 | 175.997 | 0.951 | 160.291 | 0.866 |

**Optimization Effect Analysis**:
- End-to-end performance: 187.68μs, reduced by **28.9%** compared to Case 4
- MTE2 duration: Decreased from 250.528μs to 175.997μs, reduced by **29.8%**
- Vector instruction duration: 12.846μs, unchanged

**Principle Description**:
- **L2 Cache Role**:
  - L2 Cache is the cache layer between AI Core and HBM
  - Repeatedly accessed data can be read from L2 Cache, which is faster
- **Streaming Access Characteristics**:
  - Add input data is only read once, with no data reuse

**Performance Optimization Recommendation**:
> **Reasonably adopt L2 Cache bypass**
> 
> 1. For input data read only once (such as x, y in this example), set `SetL2CacheHint(CACHE_MODE_DISABLE)`
> 2. For data requiring repeated access (such as convolution weights), retain L2 Cache
> 3. Users are recommended to configure optimization based on measured data. In actual model and training scenarios, reasonable configuration considering upstream and downstream is needed

**Next Optimization Direction**:
- Transfer efficiency has improved, but vector instruction efficiency still has optimization space
- Can try optimizing UB memory layout to avoid Bank Conflict
---

### Case 6: Double Buffer + L2 Cache Bypass + Bank Conflict Avoidance

**Implementation**: Refer to `KernelAdd::ProcessDoubleBufferBankConflict()` function implementation

Building on double buffer + L2 Cache bypass, optimize memory address layout to avoid UB (Unified Buffer) Bank Conflict, achieving optimal performance.

**Key Code**:
```cpp
// Set L2 Cache bypass
xGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
yGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);

// Optimized address layout (Bank Conflict avoidance)
static constexpr uint32_t xAddrPingBC = 0;
static constexpr uint32_t yAddrPingBC = BANK_CONFLICT_DATA_COPY_LEN * sizeof(half);  // Offset by 256B; note: sizeof(half)=2B. Use sizeof(float) for float scenarios.
static constexpr uint32_t xAddrPongBC = MAX_DATA_COPY_LEN * sizeof(half) * 2;
static constexpr uint32_t yAddrPongBC = xAddrPongBC + BANK_CONFLICT_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t zAddrPingBC = MAX_DATA_COPY_LEN * sizeof(half) * 4;
static constexpr uint32_t zAddrPongBC = zAddrPingBC + BANK_CONFLICT_DATA_COPY_LEN * sizeof(half);
```

**Example Configuration**:
- Row direction split into 48 parts, evenly distributing data across 48 cores
- `dataCopyLen = 16256` is the number of data elements per split
- Single `DataCopy` transfer data volume is 32512 Byte
- Single `Add` processes two input `Tensor`s, total processed data volume is 65024 Byte
- Split the data to be processed in half, enabling data transfer and Vector computation to execute in parallel

**Memory Layout Optimization**:

For Atlas A2/A3 Series Products, UB size is 192KB, containing 16 Bank Groups, each Bank Group containing 3 Banks, each Bank size 4KB, composed of 128 rows, each row length 32B.

UB Bank memory layout before optimization (that is, Case 5)
<img src="figures/UBBankConflict.png" width="100%">

This results in simultaneous read-write conflicts within one bank, read-read conflicts within one bank group, and write-write conflicts.

Optimized UB Bank memory layout (Case 6)
<img src="figures/UBBankConflictResolution.png" width="100%">

Since vec instructions read 256B of data per beat (that is, simultaneously reading 8 blocks of data), the xping and yping start addresses in the figure above are exactly offset by 256B, effectively resolving UB bank conflicts.

**Bank Conflict Details**:
- UB is divided into multiple Bank Groups; simultaneous read/write to the same Bank Group causes conflicts
- By adjusting dataCopyLen (16384→16256), data start address offset is created
- Ensures data accessed per vec instruction beat is distributed across different Bank Groups

**Performance Data**:

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 184.52 | 178.49 | 6.954 | 0.039 | 3.424 | 0.019 | 171.611 | 0.961 | 121.442 | 0.68 |

**Optimization Effect Analysis**:
- End-to-end performance: 184.52μs, reduced by **1.7%** compared to Case 5
- Vector instruction duration: Decreased from 12.846μs to 6.954μs, reduced by **45.9%**
- MTE3 duration: Decreased from 160.291μs to 121.442μs, reduced by **24.2%**

**Principle Description**:
- **Bank Conflict Problem**:
  - UB (Unified Buffer) is divided into multiple Bank Groups
  - If data read/written by vector instructions in one operation falls on the same Bank, read-write conflicts occur
  - If data read/written by vector instructions in one operation falls on the same Bank Group, read-read or write-write conflicts occur
  - Bank Conflict causes memory access serialization, reducing vector instruction efficiency
- **Solution**:
  - Reduce dataCopyLen (16384→16256), creating data start address offset
  - Redesign memory layout to ensure simultaneously accessed data is distributed across different Banks

**Performance Optimization Recommendation**:
> **Optimize UB memory layout to avoid Bank Conflict**
> 
> 1. When `aiv_vec_time` is abnormally high, Bank Conflict may exist
> 2. By adjusting dataCopyLen or memory layout offset, distribute data across different Bank Groups 
> 3. Bank Conflict optimization provides obvious benefits for vector-bound scenarios

**Final Performance Summary**:
- Compared to baseline Case 0: Performance improved **6718.5x** (1239689.1μs → 184.52μs)
- Compared to single-core vector Case 1: Performance improved **37.4x** (6909.6μs → 184.52μs)


---

## Performance Comparison Summary

### Atlas A2 Training Series Performance Comparison
The table below shows performance data comparison for this example running on Atlas A2 Training Series Products:

| Case | Optimization Strategy | Cores | dataCopyLen | Task Duration(μs) | aiv_vec_time(μs) | Theoretical Vector Duration (μs) | Read Bandwidth (TB/S) | Read-Write Mixed Bandwidth (TB/S) | End-to-End Duration vs Case 0 |
|:---:|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 0 | Single-core scalar (baseline) | 1 | 4096 | 1239689.1 | NA | 283.405 | 0.0454 | NA | 1x |
| 1 | Single-core vector | 1 | 4096 | 6909.6 | 761.65 | 283.405 | 0.0432 | NA | 179.4x |
| 2 | Multi-core even splitting | 48 | 4096 | 306.58 | 15.885 | 5.904 | 1.2457 | NA | 4043.6x |
| 3 | Increase transfer granularity | 48 | 16384 | 268.5 | 12.845 | 5.904 | 1.4560 | NA | 4617.1x |
| 4 | Double buffer | 48 | 16384 | 264.02 | 12.846 | 5.904 | NA | 1.6072 | 4695.4x |
| 5 | L2 Cache bypass | 48 | 16384 | 187.68 | 12.846 | 5.904 | NA | 2.2755 | 6605.3x |
| 6 | Bank Conflict optimization | 48 | 16256 | 184.52 | 6.954 | 5.904 | NA | 2.3456 | 6718.5x |

The "theoretical vector duration" in the table represents the theoretical execution time considering only Vector computation itself under the current core count configuration. The performance data of this example was obtained running on Atlas A2 Training Series Products. This processor processes 128 half data per cycle, with clock frequency 1.85GHz. The theoretical vector duration formula is:
$$
T_{\text{theory}} = \frac{M \times N}{128 \times 1.85 \times 10^9 \times \text{core count}}
$$

For example, 48 core scenario:
$$
T_{\text{theory}} = \frac{8192 \times 8192}{128 \times 1.85 \times 10^9 \times 48} = \frac{67108864}{1.13664 \times 10^{13}} \approx 5.904 \times 10^{-6} \text{ s} = 5.904 \text{ μs}
$$

It can be seen that Case 6 aiv_vec_time is 6.954 μs, already very close to the theoretical duration for the 48 core scenario.

Cases 0-3 do not enable double buffer, with data transfer executing serially, using read bandwidth to measure performance. Starting from Case 4, double buffer is enabled, at which point mte2 utilization is high and read-write behavior occurs in parallel, so read-write mixed bandwidth is estimated by total read-write data volume divided by $T_{mte2}$. The read bandwidth formula is:
$$
BW_{read} = \frac{D_{read}}{T_{mte2}}
$$

The read-write mixed bandwidth formula is:
$$
BW_{rw} = \frac{D_{read} + D_{write}}{T_{mte2}}
$$

Where:
- $D_{read} = M \times N \times sizeof(half) \times 2$ is total read data volume (x and y two input matrices)
- $D_{write} = M \times N \times sizeof(half)$ is total write data volume (z output matrix)
- $T_{mte2}$ is aiv_mte2_time (GM→UB transfer duration, μs)
- After enabling double buffer, mte2 and mte3 pipeline in parallel; from this example data, Case 4-6 mte2 utilization rates are 97.3%, 95.1%, 96.1% respectively, so $T_{mte2}$ is used as the main path time for estimation here

Taking Case 3 as an example ($M=N=8192$, $T_{mte2}=184.369\mu s$):
$$
BW_{read} = \frac{8192 \times 8192 \times 2 \times 2}{184.369 \times 10^{-6}} = \frac{268435456}{184.369 \times 10^{-6}} \approx 1.4560 \times 10^{12} \text{ B/s} \approx 1.4560 \text{ TB/s}
$$

Taking Case 6 as an example ($T_{mte2}=171.611\mu s$):
$$
BW_{rw} = \frac{8192 \times 8192 \times (2+1) \times 2}{171.611 \times 10^{-6}} = \frac{402653184}{171.611 \times 10^{-6}} \approx 2.3456 \times 10^{12} \text{ B/s} \approx 2.3456 \text{ TB/s}
$$

The read-write mixed bandwidth is higher than 1.8 TB/s because this metric is not pure read bandwidth, but combined read and write bandwidth. After double buffer is enabled, the read and write pipelines run in parallel. In addition, z writes hit the L2 Cache, which provides high bandwidth. Therefore, when mte2 is used as the mixed read-write time, the numerator includes the total amount of data read and written, so the resulting mixed bandwidth can exceed 1.8 TB/s.

### Ascend 950 Series Performance Comparison

The table below shows performance data comparison for this example running on Ascend 950 Series Products:

| Case | Optimization Strategy | Cores | dataCopyLen | Task Duration(μs) | aiv_vec_time(μs) | Theoretical Vector Duration (μs) | Read Bandwidth (TB/S) | Read-Write Mixed Bandwidth (TB/S) | End-to-End Duration vs Case 0 |
|:---:|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 0 | Single-core scalar (baseline) | 1 | 4096 | 1228475.435 | NA | 317.750 | 0.034 | NA | 1x |
| 1 | Single-core vector | 1 | 4096 | 8924.021 | 943.362 | 317.750 | 0.035 | NA | 137.7x |
| 2 | Multi-core even splitting | 64 | 4096 | 310.489 | 15.465 | 4.965 | 1.184 | NA | 3956.6x |
| 3 | Increase transfer granularity | 64 | 21760 | 251.684 | 11.121 | 4.965 | 1.454 | NA | 4881.0x |
| 4 | Double buffer | 64 | 21760 | 247.665 | 10.976 | 4.965 | NA | 1.712 | 4960.2x |
| 5 | L2 Cache bypass | 64 | 21760 | 182.04 | 10.997 | 4.965 | NA | 2.256 | 6748.4x |

The main reason `dataCopyLen` is increased on Ascend 950 Series is that UB capacity increases. In double buffer scenarios, this example needs to simultaneously place `x/y/z` Ping-Pong totaling 6 buffer blocks, so single block available space can be approximated as `UBSIZE/6`:
- Atlas A2/A3 Series: `192KB / 6 = 32KB`, corresponding to `32KB / 2B = 16384` `half` elements
- Ascend 950 Series: `256KB / 6 ≈ 42.67KB`, corresponding to approximately `21840` `half` elements

Therefore on Ascend 950 Series, estimated by capacity upper limit, single block `dataCopyLen` can be increased to approximately `21840` `half` data elements.
In actual implementation, it is recommended that users align both transfer granularity and computation granularity to 512B for more stable performance. Therefore cases 3-5 use `dataCopyLen=21760`, corresponding to `21760 * 2B = 43520B = 85 * 512B` bytes.

On Ascend 950 Series Products, BANK arrangement is different. This example does not need to consider Bank conflicts for now, so only case 0 to case 5 performance data is listed. This processor processes 128 half data per cycle, with clock frequency 1.65GHz and AIV core count 64, so theoretical vector duration is:
$$
T_{\text{theory}} = \frac{8192 \times 8192}{128 \times 1.65 \times 10^9 \times 64} = \frac{67108864}{1.35168 \times 10^{13}} \approx 4.965 \times 10^{-6} \text{ s} = 4.965 \text{ μs}
$$

The reasons why the Ascend 950 Series does not reach the theoretical vector duration peak are as follows:

- The theoretical value covers only pure Vector computation and excludes data transfer, event synchronization, pipeline scheduling, and other overhead. In the measured Case 5, `aiv_vec_time=10.997μs`, Task Duration is `182.04μs`, and `aiv_mte2_ratio` reaches 98.3%. This indicates that the end-to-end bottleneck is bandwidth rather than pure computation.
- The main benefits of RegBase on the Ascend 950 Series come from reducing redundant Load/Store operations, reusing registers, and increasing the proportion of independent instructions that can execute concurrently. The core computation in this Add example is essentially a single Add instruction, with a short computation chain and limited opportunities for fusion or dual issue, so it cannot fully realize the benefits of RegBase.

### Optimization Summary

| Optimization Method | Core Principle | Applicable Scenario |
|:---|:---:|:---|
| Scalar→Vector | Vector instructions process multiple elements in parallel | All computation-intensive operators |
| Single-core→Multi-core | Multi-core parallelism, load balancing | Large data volume scenarios |
| Increase transfer granularity | Reduce transfer count, amortize startup overhead | Transfer-intensive scenarios |
| Double buffer | Transfer and computation pipeline parallelism | When computation and transfer times are similar |
| L2 Cache bypass | Avoid Cache pollution, reduce overhead | Streaming access (read only once) |
| Bank Conflict optimization | Optimize memory layout, avoid memory access conflicts | Vector-bound scenarios |

---

## Build and Run

- Switch Case

  Specify the case to build through `-DSCENARIO_NUM=N` during cmake build:

  ```bash
  cmake -DSCENARIO_NUM=6 -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..   # Build case 6 (can be replaced with 0-6)
  ```

  Case descriptions:
  - `0`: Single-core scalar version
  - `1`: Single-core vector version
  - `2`: Multi-core even splitting (dataCopyLen=4096)
  - `3`: Multi-core even splitting (large block transfer)
  - `4`: Double buffer optimization
  - `5`: Double buffer + L2Cache bypass
  - `6`: Double buffer + L2Cache bypass + Bank Conflict avoidance

- Configure environment variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
    
- Run the example

  Run the following commands in this example directory.
  ```bash 
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake -DSCENARIO_NUM=6 -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run (using the case specified at build time)
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic correctness
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DSCENARIO_NUM=6 -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DSCENARIO_NUM=6 -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.
  

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0` (default), `1`, `2`, `3`, `4`, `5`, `6` | Performance optimization case number |
  
  The following execution result indicates that the accuracy comparison succeeded.
  ```bash
  error ratio: 0.0000, tolerance:0.0001
  test pass!
  ```


## Function debugging

### printf

This interface provides formatted output function in CPU domain/NPU domain debugging scenarios.

In the implementation code on the operator kernel side, call the [printf](../../../../../docs/zh/api/Utils-API/tuning_interface/printf.md) interface to print relevant content.

Examples are as follows:

```cpp
AscendC::printf("add blockIdx=%d\n", AscendC::GetBlockIdx());
```

> **Note:** The printing function of the printf (PRINTF) interface will have a certain impact on the actual performance of the operator and is usually used during the debugging phase. Developers can turn off the printing function as needed by setting `ASCENDC_DUMP=0`.

### DumpTensor

For operators developed based on the operator project, you can use this interface Dump to specify the contents of [LocalTensor](../../../../../docs/zh/api/SIMD-API/basic_api/data_structures/LocalTensor/LocalTensor_intro.md). It also supports printing of customized additional information (only uint32\_t data type information is supported), such as printing the current line number, etc.

In the operator kernel side implementation code, where Tensor data needs to be printed, call the [DumpTensor](../../../../../docs/zh/api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor.md) interface to print relevant content. An example is as follows:

```cpp
// Vector calculation: z = x + y
AscendC::Add(zLocal, xLocal, yLocal, blockLength);
AscendC::DumpTensor(zLocal, 1, 32);
```

> **Note:** The printing function of the DumpTensor interface will have a certain impact on the actual performance of the operator and is usually used during the debugging phase. Developers can turn off the printing function by setting `ASCENDC_DUMP=0` as needed.

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

View the detailed performance analysis results:

```bash
# View Task Duration and other metrics
cat ./OPPROF_*/PipeUtilization.csv
```

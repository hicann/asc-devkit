# ND2NZ Bank Conflict Tuning Example

## Overview

This example uses an `8192 × 8192` half ND matrix to globally compact NZ layout conversion to introduce how bank conflicts arise during the vector write phase on UB and how to avoid them. The tuning process is divided into two steps (case 1-2), comparing performance by adjusting the single parameter `dstNzC0Stride`.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── bank_conflict_nd2nz
│   ├── bank_conflict_nd2nz.asc    // Ascend C example implementation (including 2 cases)
│   ├── bank_conflict_nd2nz_3510.h // dav-3510 Regbase ND2NZ implementation
│   ├── CMakeLists.txt             // Build project file
│   ├── data_utils.h               // Data read/write functions
│   ├── figures                    // Illustrations
│   ├── README.md                  // Example documentation
│   └── scripts
│       ├── gen_data.py            // Input data and ground truth generation script
│       └── verify_result.py       // Ground truth comparison script
```

## Example Description

**Example Function**:

The example implements a fixed shape `8192 × 8192` half ND matrix to globally compact NZ layout conversion.

The essence of ND → NZ conversion is a large-scale vector rearrangement write: "scattering" the 8 C0 blocks (each C0 block = 16 half = 32B = 1 DataBlock) from each row within a tile to the corresponding row slots across 8 different C0 columns in the NZ layout. `dstNzC0Stride` controls the spacing between adjacent C0 column landing points within UB, and is the core knob for bank conflicts:

- x: input, shape `[8192, 8192]`, half, ND row-major layout
- z: output, shape `[8192, 8192]`, half, globally compact NZ layout

**Block and Core Distribution**:

The current implementation first splits the `8192 × 8192` input along N/D directions to each Vector core, with each block responsible for a row-split × column-split region. The split method is configured by architecture: 2201 uses `6 row splits × 8 column splits = 48` blocks, 3510 uses `8 row splits × 8 column splits = 64` blocks. Within each block, data is further split into `144 × 128` half small tiles for pipelining; since `8192` is not divisible by `144`, the last global row tile has an effective height of `128`, handled through `actualTileH` in the code.

```text
2201:
      colSplit →  0       1       2     ...       7

rowSplit ↓      ┌───────┬───────┬───────┬─────┬───────┐
      0         │  #0   │  #1   │  #2   │ ... │  #7   │
      1         │  #8   │  #9   │ #10   │ ... │ #15   │
     ...        │                  ...                │
      5         │ #40   │ #41   │ #42   │ ... │ #47   │
                └───────┴───────┴───────┴─────┴───────┘

3510:
      colSplit →  0       1       2     ...       7
rowSplit ↓      ┌───────┬───────┬───────┬─────┬───────┐
      0         │  #0   │  #1   │  #2   │ ... │  #7   │
      1         │  #8   │  #9   │ #10   │ ... │ #15   │
     ...        │                  ...                │
      7         │ #56   │ #57   │ #58   │ ... │ #63   │
                └───────┴───────┴───────┴─────┴───────┘

```

**Key Parameters**:

| Parameter | Value | Meaning |
|:---:|:---:|:---|
| `rowSplits` | dav-2201 scenario = 6 / dav-3510 scenario = 8 | N direction split count, different architectures have different split logic, dav-2201 refers to Atlas A2/A3 Series Products, dav-3510 refers to Ascend 950PR/Ascend 950DT |
| `colSplits` | 8 | D direction split count |
| `totalBlocks` | dav-2201 scenario = 48 / dav-3510 scenario = 64 | Number of blocks to launch, equals `rowSplits * colSplits` |
| `tileH` | 144 | Maximum N direction rows per transfer tile; tail block uses `actualTileH` = 128 |
| `tileW` | 128 | D direction half count per transfer tile |
| `C0_ELEMS` | 16 | Fixed layout parameter: number of half elements in one C0 block, also the number of half elements in one DataBlock, not an adjustable tiling parameter |
| `vecLenHalf` | 128 | Tiling parameter: number of half elements processed by a single vector instruction |
| `tileC0Cols` | 8 | Derived parameter: D direction C0 column count per transfer tile, equals `tileW / C0_ELEMS` |
| `dstNzNStride` | 1 | N direction stride within single transfer UB, in DataBlock units |
| `dstNzC0Stride` | case1 = 144 / case2 = 145 | Spacing between adjacent C0 column start positions within single transfer UB, in DataBlock units. Corresponds to `dstStride` in the `CopyRepeatParams` of the `Copy()` interface — the address stride of datablocks within the same iteration. This example compares bank conflict scenarios by switching between 144 / 145. |
| `vecLenDbs` | 8 | Derived parameter: number of DataBlocks processed by a single vector instruction, equals `vecLenHalf / C0_ELEMS` |

> **Terminology: DataBlock**
>
> DataBlock is the **data unit processed by a single NPU vector computation instruction**, with a size of **32 bytes** (16 half elements for the half type); a single vector instruction can process multiple DataBlocks per Repeat. All stride parameters (`dstNzNStride` / `dstNzC0Stride`) below use DataBlock as the unit; "32B block" in the text refers to DataBlock.

**ND → NZ Data Layout**:

The data layouts of ND and NZ are shown in the figure.

**Figure: ND and NZ Data Layout Diagram**

<img src="figures/nd2nz.png" width="80%">

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

---

### Atlas A2/A3 Series Products (dav-2201)

> Select Atlas A2/A3 Series Products at build time with `-DCMAKE_ASC_ARCHITECTURES=dav-2201`.

#### UB Bank Structure and Conflict Types

On Atlas A2/A3 Series Products, the Unified Buffer is 192KB total, divided into 48 banks (each bank has 128 rows × 32B = 4KB); these 48 banks are further organized into **16 bank groups**, each bank group containing 3 banks. The Vector computation unit can read or write one row of data from each bank group per beat (one instruction cycle). When multiple read/write operations in one beat fall on the same bank or bank group, the hardware cannot process them all in one cycle and must queue them, causing a bank conflict.

**Figure: Atlas A2/A3 UB Bank Structure Diagram** (arrow direction indicates memory layout order)

<img src="figures/ubBankStruct2201.png" width="80%">

Bank conflicts are mainly categorized into three types:

- **Read-Write Conflict**: Read and write operations simultaneously attempt to access the same bank;
- **Write-Write Conflict**: Multiple write operations simultaneously attempt to access the same bank group;
- **Read-Read Conflict**: Multiple read operations simultaneously attempt to access the same bank group.

The ND→NZ conversion in this example primarily triggers **write-write conflicts**: when writing NZ vectors, one beat writes 8 DataBlocks, and the bank group of the landing points is determined by `dstNzC0Stride`. The following compares the two cases.

#### Case 1: Compact Layout

**Implementation**: Build with `-DSCENARIO_NUM=1`.

The code takes up to `144×128` `Tile` for ND→NZ rearrangement each time. The transposed 8 C0 columns are **sequentially placed end-to-end in UB**, which is the "natural approach" for NZ layout. Tail blocks use `actualTileH=128`, but the maximum stride between adjacent C0 columns in UB remains fixed at `tileH=144`.

**Key Code**:

**ND2NZ Implementation**:

`Copy()` has built-in repeat; a single Copy covers `actualTileH` rows × 8 C0 columns; for non-tail blocks this is `144×128` half, one row of 8 C0 columns requires only 1 `Copy()`, so loop in the `tileW` direction:

```cpp
AscendC::CopyRepeatParams copyParams;
copyParams.dstStride     = dstNzC0Stride;    // = 144, spacing between adjacent C0 column start positions
copyParams.srcStride     = 1;
copyParams.dstRepeatSize = 1;
copyParams.srcRepeatSize = tileW / C0_ELEMS;

for (uint32_t k = 0; k < tileW / vecLenHalf; ++k) {
    AscendC::Copy(nzBuf[k * vecLenDbs * dstNzC0Stride * C0_ELEMS],
                  ndBuf[k * vecLenHalf],
                  static_cast<uint64_t>(vecLenHalf),
                  static_cast<uint8_t>(actualTileH),
                  copyParams);
}
```

The layout of 8 datablock data fetched per beat in the NZ matrix is shown below. For non-tail blocks, two originally adjacent datablocks in the NZ layout require a starting address spacing of `144 * 32B`:

**Figure: Case 1 Compact Layout ND to NZ Diagram**

<img src="figures/datand2nzS1.png" width="80%">

**Ping-Pong Pipeline**:

The example enables a double buffer mechanism in `Process`, allocating two sets of buffers `ndPing/ndPong` and `nzPing/nzPong` on UB simultaneously. Adjacent tiles alternate between the two buffer sets. Each buffer set uses an independent EVENTID.

**UB layout order**: 4 buffers are arranged as `[ndPing | ndPong | nzPing | nzPong]`.

> The Ping-Pong pipeline implementation in Case 2 / Ascend 950PR/Ascend 950DT cases is identical and will not be repeated.

**Conflict Principle**:

The 16 bank groups of UB can each accept writes in parallel; addresses are assigned sequentially in 32B blocks cyclically: the 1st 32B block belongs to bank group 0, the 2nd to bank group 1, ..., the 17th returns to bank group 0 starting a new cycle. **When multiple writes in one beat all target the same bank group, they must queue**.

A single copy writes 8 32B blocks to UB per beat, with adjacent landing points separated by `dstNzC0Stride = 144` 32B blocks. **144 is exactly a multiple of 16** — every 144-step jump returns to the same bank group. The result is all 8 writes squeezed into bank group 0, hardware can only process them sequentially, and a single copy stretches from **1 beat to 8 beats**, causing aiv_vec_time to inflate significantly.

**S1 Landing Point Diagram**:

```text
Landing points of 8 writes from a single copy() on UB:
  DataBlock 0 →    0th 32B block  → bank group 0
  DataBlock 1 →  144th 32B block  → bank group 0   ◀ same bank group
  DataBlock 2 →  288th 32B block  → bank group 0   ◀ same bank group
  DataBlock 3 →  432nd 32B block  → bank group 0   ◀ same bank group
  DataBlock 4 →  576th 32B block  → bank group 0   ◀ same bank group
  DataBlock 5 →  720th 32B block  → bank group 0   ◀ same bank group
  DataBlock 6 →  864th 32B block  → bank group 0   ◀ same bank group
  DataBlock 7 → 1008th 32B block  → bank group 0   ◀ same bank group
```

The figure below shows the blue portion as data read from NDBUF per beat and the red portion as data written to NZBUF per beat. All 8 datablocks fall into the same bank group:

**Figure: Atlas A2/A3 Case 1 UB Bank Group Conflict Diagram**

<img src="figures/s1bank2201.png" width="80%">

**Performance Data** (Atlas A2/A3 Series Products, current `144×128` configuration, 48 cores):

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 151.42 | 138.00 | **47.790** | 0.346 | 6.586 | 0.048 | 113.856 | 0.825 | 76.760 | 0.556 |

**Optimization Effect Analysis**:
- `aiv_vec_time = 47.790 μs`.
- The main conflict point is in the Vec write phase: 8 32B blocks from copy all hit bank group 0.

#### Case 2: Allocate One Extra Row for nzBuf

**Implementation**: Build with `-DSCENARIO_NUM=2`.

Based on Case 1 compact layout, **adjust `dstNzC0Stride` from `tileH` to `tileH + 1`**, equivalent to allocating one extra row of space for `nzBuf`, allowing vector write landing points to rotate across different bank groups.

**Key Code**:

```cpp
constexpr uint32_t dstNzC0Stride = (SCENARIO == 1) ? tileH : (tileH + 1);
```

Ping-Pong pipeline is identical to Case 1, see above.

**Avoidance Principle**:

Changing `dstNzC0Stride` from 144 to 145 allocates one extra row of space on UB compared to Case 1, with adjacent write landing points separated by 145 32B blocks. Every 145 steps, in 16 bank groups, completes 9 full rounds plus 1 extra step, with landing points entering bank group 0, 1, 2, ..., 7 sequentially. **8 writes are distributed to 8 different bank groups**, hardware can process them concurrently, and a single copy completes in **1 beat**.

The extra row of allocated space is not written back to GM as valid data; it only changes the bank distribution of write landing points within UB.

**Figure: Case 2 ND to NZ Diagram After Allocating One Extra Row for nzBuf**

<img src="figures/datand2nzS2.png" width="80%">

**S2 Landing Point Diagram**:

```text
Landing points of 8 writes from a single copy() on UB:
  DataBlock 0 →    0th 32B block  → bank group 0
  DataBlock 1 →  145th 32B block  → bank group 1
  DataBlock 2 →  290th 32B block  → bank group 2
  DataBlock 3 →  435th 32B block  → bank group 3
  DataBlock 4 →  580th 32B block  → bank group 4
  DataBlock 5 →  725th 32B block  → bank group 5
  DataBlock 6 →  870th 32B block  → bank group 6
  DataBlock 7 → 1015th 32B block  → bank group 7
```

**Figure: Atlas A2/A3 Case 2 UB Bank Landing Point Diagram**

<img src="figures/s2bank2201.png" width="80%">

**UB Usage Change**:

`ndBuf` usage = `tileH * tileW * sizeof(half) = 144 * 128 * 2 = 36864 B = 36 KB`

`nzBuf` DataBlock count reflects actual UB allocated space, not the valid data written back to GM. S1 allocates compactly at 144 rows, which is `144 × 8 = 1152` DataBlocks; S2 has `dstNzC0Stride = 145`, equivalent to allocating one extra row for `nzBuf`, so it is `145 × 8 = 1160` DataBlocks.

| Scenario | `ndBuf` Usage | `nzBuf` DataBlock Count | `nzBuf` Usage |
|:---:|:---:|:---:|:---:|
| S1 (stride=144) | 36864 B = 36 KB | 1152 | 1152 × 32 B = 36864 B = 36 KB (compact) |
| S2 (stride=145) | 36864 B = 36 KB | 1160 | 1160 × 32 B = 37120 B ≈ 36.3 KB (145 rows allocated) |

The extra row of allocated space is skipped during the UB→GM MTE3 phase by `outParams.srcStride = dstNzC0Stride - tileH`, and is not written back to GM.

With double buffering, each buffer set contains one `ndBuf` and one `nzBuf`. S1 total UB usage is `2 × (36864 B + 36864 B) = 147456 B = 144 KB`, S2 total UB usage is `2 × (36864 B + 37120 B) = 147968 B ≈ 144.5 KB`, which does not exceed UB Bank memory.

**Performance Data** (Atlas A2/A3 Series Products, current `144×128` configuration, 48 cores):

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 144.00 | 131.44 | **7.133** | 0.054 | 7.760 | 0.059 | 115.405 | 0.878 | 76.532 | 0.582 |

**Optimization Effect Analysis**:
- `aiv_vec_time` from 47.790 → 7.133 μs (**6.70×** improvement), Vec write conflicts are significantly reduced after bank group distribution.
- Cost is 8 × 32B = 256B extra UB space per tile (approximately 0.7% waste), fully acceptable.

---

### Ascend 950PR/Ascend 950DT (dav-3510)

> Select Ascend 950PR/Ascend 950DT at build time with `-DCMAKE_ASC_ARCHITECTURES=dav-3510`.

#### UB Bank Structure and Conflict Types

On Ascend 950PR/Ascend 950DT, the Unified Buffer is 256KB total, divided into 16 banks (each bank has 512 rows × 32B = 16KB); these 16 banks are further organized into **8 bank groups**, each bank group containing 2 banks (bank `i` and bank `i+8` belong to bank group `i`, that is, `bank group = bank % 8`). The Vector computation unit can read or write one row of data from each bank group per beat.

**Figure: Ascend 950PR/Ascend 950DT UB Bank Structure Diagram**

<img src="figures/ubBankStruct3510.png" width="80%">

**Address uses low-bit interleaving**: Consecutive addresses in UB cycle across bank0~bank15 in 32B units — the 1st 32B block falls on bank0, the 2nd on bank1, ..., the 16th on bank15, the 17th returns to the next row of bank0, and so on.

**Figure: Ascend 950PR/Ascend 950DT UB Bank Memory Layout Diagram** (arrow direction indicates memory layout order)
<img src="figures/UB-3510.png" width="80%">

Bank conflicts are mainly categorized into three types:

- **Read-Write Conflict**: Read and write operations simultaneously attempt to access the same bank;
- **Write-Write Conflict**: Multiple write operations simultaneously attempt to access the same bank group;
- **Read-Read Conflict**: Two read operations simultaneously attempt to access the same bank, or more than two read operations simultaneously attempt to access the same bank group.

The ND→NZ conversion in this example primarily triggers **write-write conflicts**: when writing NZ vectors, one beat writes 8 DataBlocks, and the bank group of the landing points is determined by `dstNzC0Stride`. The following compares the two cases.

#### Case 1: Compact Layout

**Implementation**: Build with `-DSCENARIO_NUM=1`.

The code takes up to `144×128` `Tile` for ND→NZ rearrangement each time. The transposed 8 C0 columns are **sequentially placed end-to-end in UB**, which is the "natural approach" for NZ layout. Tail blocks use `actualTileH=128`.

**Key Code**:

```cpp
constexpr uint32_t dstNzC0Stride = (SCENARIO == 1) ? tileH : (tileH + 1);
```

The Ping-Pong pipeline implementation is identical to Atlas A2/A3 Case 1 (see [Atlas A2/A3 → Case 1 → Ping-Pong Pipeline](#case-1-compact-layout)).

**ND2NZ Implementation**:

Ascend 950PR/Ascend 950DT implements Nd2Nz rearrangement based on Reg programming interface, using `__simd_vf__`, with outer vector column group and inner row loop:

```cpp
for (uint16_t k = 0; k < vecsPerRow; ++k) {
    const uint32_t dstColGroupOffset = k * vecLenDbs * dstNzC0Stride * C0_ELEMS;

    for (uint16_t r = 0; r < actualTileH; ++r) {
        AscendC::Reg::LoadAlign<half>(reg, srcAddr + r * tileW + k * vecLenHalf);

        __ubuf__ half* dstRowAddr = dstAddr + dstColGroupOffset + r * C0_ELEMS;
        AscendC::Reg::StoreAlign<half, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dstRowAddr, reg, dstNzC0Stride, mask);
    }
}
```

The layout of 8 datablock data fetched per beat in the NZ matrix is shown below. For non-tail blocks, two originally adjacent datablocks in the NZ layout require a starting address spacing of `144 * 32B`:

**Figure: Case 1 Compact Layout ND to NZ Diagram**

<img src="figures/datand2nzS1.png" width="80%">

**Conflict Principle**:

According to low-bit interleaving rules, each 32B block jump changes the physical bank (16 steps complete one cycle), and every 8 steps completes one bank group cycle. **When multiple writes in one beat all target the same bank group, they must queue**.

A single `Reg::StoreAlign<DATA_BLOCK_COPY>` writes 8 32B blocks to UB per beat, with adjacent landing points separated by `dstNzC0Stride = 144` 32B blocks. **144 = 16 × 9, a multiple of 16** — every 144-step jump not only returns to the same bank group but returns to the exact same physical bank (bank 0). The result is all 8 writes squeezed into bank 0, hardware can only process them sequentially, and a single StoreAlign stretches from **1 beat to 8 beats**, causing aiv_vec_time to inflate significantly.

**S1 Landing Point Diagram**:

```text
Landing points of 8 writes from a single StoreAlign on UB:
  DataBlock 0 →    0th 32B block  → bank 0 → bank group 0
  DataBlock 1 →  144th 32B block  → bank 0 → bank group 0   ◀ same bank
  DataBlock 2 →  288th 32B block  → bank 0 → bank group 0   ◀ same bank
  DataBlock 3 →  432nd 32B block  → bank 0 → bank group 0   ◀ same bank
  DataBlock 4 →  576th 32B block  → bank 0 → bank group 0   ◀ same bank
  DataBlock 5 →  720th 32B block  → bank 0 → bank group 0   ◀ same bank
  DataBlock 6 →  864th 32B block  → bank 0 → bank group 0   ◀ same bank
  DataBlock 7 → 1008th 32B block  → bank 0 → bank group 0   ◀ same bank
```

**Figure: Ascend 950PR/Ascend 950DT Case 1 UB Bank Group Conflict Diagram**

<img src="figures/s1bank3510.png" width="80%">

**Performance Data** (Ascend 950PR/Ascend 950DT, current `144×128` configuration, 64 cores):

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 141.691 | 140.89 | **45.25** | 0.321 | 2.638 | 0.019 | 134.915 | 0.958 | 104.571 | 0.742 |

**Optimization Effect Analysis**:
- `aiv_vec_time = 45.25 μs`, approximately 4.95 times that of Case 2.
- Conflict point is in the Vec write phase: each StoreAlign in VEC_STORE pipeline queues for 8 beats due to bank-group write-write conflicts.

**Performance Bottleneck Identification**:
> **Bank conflict in Vec pipeline**
>
> In the Regbase architecture, `Reg::LoadAlign` uses the **VEC_LOAD** sub-pipeline and `Reg::StoreAlign` uses the **VEC_STORE** pipeline, not MTE. Standard MTE-type bank-conflict counters cannot detect this; you need to examine the vec pipeline or single instruction cycle counts.

#### Case 2: Allocate One Extra Row for nzBuf

**Implementation**: Build with `-DSCENARIO_NUM=2`.

Based on Case 1 compact layout, **adjust `dstNzC0Stride` from `tileH` to `tileH + 1`**, equivalent to allocating one extra row of space for `nzBuf`, allowing vector write landing points to rotate across different bank groups.

**Key Code**:

```cpp
constexpr uint32_t dstNzC0Stride = (SCENARIO == 1) ? tileH : (tileH + 1);
```

**Avoidance Principle**:

Changing `dstNzC0Stride` from 144 to 145 allocates one extra row of space on UB compared to Case 1, with adjacent write landing points separated by 145 32B blocks. **145 = 16 × 9 + 1**, each jump increments the physical bank index by 1, and 8 landing points fall on 8 adjacent physical banks sequentially; since `bank group = bank % 8`, these 8 banks simultaneously belong to 8 different bank groups, **8 writes are distributed exactly to 8 bank groups**, hardware can process them concurrently, and a single StoreAlign completes in **1 beat**. In this implementation, the nzBuf start position is already offset to bank 8 (see "Supplement" section below), so the 8 landing points are actually banks 8 through 15.

The extra row of allocated space is not written back to GM as valid data; it only changes the bank distribution of write landing points within UB.

**Figure: Case 2 ND to NZ Diagram After Allocating One Extra Row for nzBuf**

<img src="figures/datand2nzS2.png" width="80%">

**S2 Landing Point Diagram** (nzBuf already offset to start at bank 8):

```text
Landing points of 8 writes from a single StoreAlign on UB ("Nth 32B block" counted from nzBuf start):
  DataBlock 0 →    0th 32B block  → bank 8  → bank group 0
  DataBlock 1 →  145th 32B block  → bank 9  → bank group 1
  DataBlock 2 →  290th 32B block  → bank 10 → bank group 2
  DataBlock 3 →  435th 32B block  → bank 11 → bank group 3
  DataBlock 4 →  580th 32B block  → bank 12 → bank group 4
  DataBlock 5 →  725th 32B block  → bank 13 → bank group 5
  DataBlock 6 →  870th 32B block  → bank 14 → bank group 6
  DataBlock 7 → 1015th 32B block  → bank 15 → bank group 7
```

**Figure: Ascend 950PR/Ascend 950DT Case 2 UB Bank Group Distributed Landing Point Diagram**

<img src="figures/s2bank3510.png" width="80%">

> **Supplement: nzBuf Start Address Offset to Bank 8**
>
> On 950PR, S2 also offsets the nzBuf start address by **an additional 8 datablocks = 256B** beyond the end of ndBuf, placing nzBuf starting at bank 8. This way, one beat of V_LOAD reads ndBuf row 0 landing on banks 0~7, and V_STORE lands on banks 8~15, with non-overlapping physical banks, which can reduce same-beat V_LOAD/V_STORE read-write conflicts (read-write conflicts within the same bank).

**UB Usage Change**:

`ndBuf` usage = `tileH * tileW * sizeof(half) = 144 * 128 * 2 = 36864 B = 36 KB`

`nzBuf` DataBlock count reflects actual UB allocated space, not the valid data written back to GM. S1 allocates compactly at 144 rows, which is `144 × 8 = 1152` DataBlocks; S2 has `dstNzC0Stride = 145`, equivalent to allocating one extra row for `nzBuf`, so it is `145 × 8 = 1160` DataBlocks.

| Scenario | `ndBuf` Usage | `nzBuf` DataBlock Count | `nzBuf` Usage |
|:---:|:---:|:---:|:---:|
| S1 (stride=144) | 36864 B = 36 KB | 1152 | 1152 × 32 B = 36864 B = 36 KB (compact) |
| S2 (stride=145) | 36864 B = 36 KB | 1160 | 1160 × 32 B = 37120 B ≈ 36.3 KB (145 rows allocated) |

The extra row of allocated space is skipped during the UB→GM MTE3 phase by `outParams.srcStride = dstNzC0Stride - tileH`, and is not written back to GM.

With double buffering, each buffer set contains one `ndBuf` and one `nzBuf`. S1 total UB usage is `2 × (36864 B + 36864 B) = 147456 B = 144 KB`; S2 also introduces a 256B bank-8 start offset for each of the two `nzBuf` blocks (see "Supplement" section above), total UB usage is `2 × (36864 B + 37120 B) + 2 × 256 B = 148480 B ≈ 145 KB`, satisfying the Ascend 950PR/Ascend 950DT 256 KB UB constraint.

**Performance Data** (Ascend 950PR/Ascend 950DT, current `144×128` configuration, 64 cores):

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 140.403 | 139.68 | **9.14** | 0.065 | 2.765 | 0.020 | 135.115 | 0.967 | 116.921 | 0.837 |

**Optimization Effect Analysis**:
- `aiv_vec_time` from 45.25 → 9.14 μs (**4.95×** improvement).
- Cost is 8 × 32B = 256B extra UB space per tile (approximately 0.7% waste), fully acceptable.

## Theoretical Performance Analysis

### Atlas A2/A3 Series Products (dav-2201)

The current implementation selects `144×128`. With this configuration, S2 compared to S1 already demonstrates the benefit of avoiding Vec bank conflicts, with end-to-end duration decreasing from `151.42μs` to `144.00μs`.

| Tile | Case | dstNzC0Stride | Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio | icache_miss_rate |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 144×128 | S1 | 144 | 151.42 | 138.00 | 12254207 | **47.790** | 0.346 | 6.586 | 0.048 | 113.856 | 0.825 | 76.760 | 0.556 | 0.007 |
| 144×128 | S2 | 145 | **144.00** | 131.44 | 11671939 | **7.133** | 0.054 | 7.760 | 0.059 | 115.405 | 0.878 | 76.532 | 0.582 | 0.005 |

On Atlas A2/A3 Series Products, a single vcopy processes `128` half elements per beat, with clock frequency at `1.85GHz` and core count of `48`. The conflict-free base duration is:

$$
T_{\text{base}} = \frac{M \times N}{128 \times 1.85 \times 10^9 \times 48}
                = \frac{8192 \times 8192}{1.13664 \times 10^{13}}
                \approx 5.904\mu s
$$

In Case 1, `dstNzC0Stride = 144`, 8 writes land on the same bank group, stretching a single vector write from 1 beat to 8 beats, therefore:

$$
T_{\text{S1}} = 8 \times T_{\text{base}} \approx 47.233\mu s
$$

In Case 2, `dstNzC0Stride = 145`, 8 writes are evenly distributed across different bank groups, therefore:

$$
T_{\text{S2}} = 1 \times T_{\text{base}} \approx 5.904\mu s
$$

In Case 1, because V itself is stretched to 8 beats for writing, interference between MTE2/V is masked; in Case 2, after V is reduced to 1 beat, under the ping-pong mechanism, MTE2 writing ndPong and V reading ndPing landing on the same bank causes read-write conflicts to become visible, so the measured 7.133μs in Case 2 is approximately 21% higher than the theoretical 5.904μs. End-to-end Task Duration decreases from `151.42μs` to `144.00μs`, a reduction of approximately 4.9%; the overall result still includes MTE2/MTE3 transfer and synchronization overhead, with bank conflict optimization primarily reflected in the Vec sub-pipeline.

### Ascend 950PR/Ascend 950DT (dav-3510)

The current implementation selects `144×128`. S2 shows a significant decrease in Vec time, but the current end-to-end performance is primarily limited by MTE2/MTE3, so Task Duration only decreases from `141.691μs` to `140.403μs`.

| Tile | Case | dstNzC0Stride | Task Duration(μs) | Block Num  | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio | aiv_icache_miss_rate |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 144×128 | S1 | 144 | 141.691 | 64  | 140.89 | 14125772 | **45.25** | 0.321 | 2.638 | 0.019 | 134.915 | 0.958 | 104.571 | 0.742 | 0.006 |
| 144×128 | S2 | 145 | **140.403** | 64  | 139.68 | 14094639 | **9.14** | 0.065 | 2.765 | 0.020 | 135.115 | 0.967 | 116.921 | 0.837 | 0.006 |

On Ascend 950PR/Ascend 950DT, the Reg path processes each group of `128` half data elements, requiring one `LoadAlign` and one `StoreAlign`.

A single `LoadAlign` or `StoreAlign` processes `128` half elements per beat. `StoreAlign` and `LoadAlign` across loops can land on different physical registers without blocking each other, ideally processing `256 bytes/cycle`. With clock frequency at `1.65GHz` and core count of `64`, the base duration for processing all data in a single pass is:

$$
T_{\text{base}} = \frac{M \times N}{128 \times 1.65 \times 10^9 \times 64}
                = \frac{8192 \times 8192}{1.35168 \times 10^{13}}
                \approx 4.965\mu s
$$

The measured `aiv_vec_time` is `9.14μs`. The gap from the 4.965μs theoretical value comes from two combined factors: (1) V_STORE stride=145 increments the starting bank by +1 each time, and after multiple loop iterations it still lands on the same bank as V_LOAD, which cannot be perfectly staggered row by row with buffer offset alone; (2) vloop dual-issue, where the issue slots cannot always form the ideal "current round store + next round load" combination every cycle, causing conflicts. This example primarily demonstrates how to solve NZ write-write conflicts. The end-to-end Task Duration is already dominated by MTE2/MTE3 (`aiv_mte2_ratio` as high as 96.7%), with vec time not on the critical path.

### Optimization Summary

> **Core Conclusion**
>
> - If vector writes always land on the same bank group, write-write conflicts occur.
> - Resolve conflicts by allocating extra memory so that write landing points do not fall on the same bank group.

---

## Build and Run

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  mkdir -p build && cd build   # Create and enter the build directory
  cmake -DSCENARIO_NUM=1 -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run (using the case specified at build time)
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic correctness
  ```

  To use CPU debug or NPU simulation mode, add `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim`:

  ```bash
  cmake -DSCENARIO_NUM=1 -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # CPU debug mode
  cmake -DSCENARIO_NUM=1 -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes or scenarios. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2` | Bank conflict scenario number |

  The following execution result indicates that the accuracy comparison succeeded:

  ```bash
  test pass!
  ```

## Performance Analysis

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
cat ./OPPROF_*/PipeUtilization.csv
```

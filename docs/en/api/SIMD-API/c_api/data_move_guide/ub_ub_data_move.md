---
okftype: reference.concept
description: "Introduces the contiguous and high-dimensional slicing modes of using asc_copy_ub2ub to copy data within the Unified Buffer on NPU architecture 3510. Describes the parameter units, alignment and value ranges, address overlap determination rules, and UB capacity and PIPE_V synchronization constraints of each mode."
---
# UB to UB Data Movement

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.277849+00:00 pushedAt=2026-11-16T02:10:55.384Z nl=2 -->

NPU architecture version 3510 copies data within the UB through [asc_copy_ub2ub](../vector_datamove/asc_copy_ub2ub.md). The API runs on PIPE_V and takes effect only on AIV. It supports both contiguous and high-dimensional slicing modes, and the data format and content remain unchanged.

## General Description

| Copy Mode | Applicable Scenario | Key Parameter Unit |
| --- | --- | --- |
| Contiguous | Both source and destination are a contiguous segment of UB data. | `size`: bytes |
| High-dimensional slicing | The source or destination consists of multiple equal-length data blocks with fixed intervals between blocks. | Block length, source/destination interval: 32-byte DataBlock. |

When element selection by mask, broadcast, or data type-related rearrangement is required in the UB, Reg load, compute, and store APIs can be used in Vector Functions. See [Reg and UB Data Movement](reg_ub_data_move.md).

## Contiguous Data Movement

In contiguous mode, this API copies `size` bytes from `src` to `dst`:

```c
__aicore__ inline void asc_copy_ub2ub(__ubuf__ void* dst,
                                      __ubuf__ void* src,
                                      uint32_t size)
```

- Both source and destination addresses must be 32-byte aligned.
- The value range of `size` is [32, 65535×32], and it must be an integer multiple of 32.
- When `size` is not aligned, the hardware uses integer division (`size / 32`) to determine the number of complete blocks to copy, and the trailing data of less than 32 bytes is not copied.
- In contiguous mode, the valid source and destination ranges must not overlap.

## High-Dimensional Slicing Data Movement

In high-dimensional slicing mode, `burst_count` data blocks are moved, with each data block containing `burst_len` DataBlocks:

```c
__aicore__ inline void asc_copy_ub2ub(__ubuf__ void* dst,
                                      __ubuf__ void* src,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t src_gap,
                                      uint16_t dst_gap)
```

- `burst_len` is in units of 32-byte DataBlocks.
- `src_gap` and `dst_gap` represent the interval from the end address of the previous data block to the start address of the next data block, also in DataBlock units.
- The source access span is `(burst_count - 1) * (burst_len + src_gap) + burst_len` DataBlocks, and the destination access span is calculated in the same way using `dst_gap`.
- The source DataBlocks and destination DataBlocks that actually participate in the movement must not overlap. If the inter-block gaps keep the valid DataBlocks disjoint, the overall source and destination address spans may overlap.

## Address Overlap Example

Assume each data block is 2 DataBlocks long and 2 blocks are moved in total:

- When `src_gap=1`, the valid source blocks are DataBlock 0, 1 and 3, 4.
- When `dst_gap=2`, the valid destination blocks are DataBlock 0, 1 and 4, 5.

To determine whether the movement is supported, compare the valid source and destination DataBlocks block by block instead of comparing only the two overall spans. The API does not provide overlap-copy semantics similar to `memmove`; when moving overlapping regions, use non-overlapping temporary UB space in stages and complete synchronization before reusing the space.

## Capacity and Synchronization

- The source and destination addresses plus the actual access spans must not exceed the allocated UB range. The nominal UB capacity is 256 KB, and the actual available capacity is also affected by the SIMD VF stack, reserved space, and SIMT Data Cache.
- `asc_copy_ub2ub` runs on PIPE_V. On NPU architecture version 3510, the execution order within PIPE_V is guaranteed by hardware, so no additional event synchronization is required between multiple `asc_copy_ub2ub` calls.
- When the destination UB is subsequently read by PIPE_MTE3 or another pipeline, use [asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md) to establish the dependency from PIPE_V to the consumer pipeline.

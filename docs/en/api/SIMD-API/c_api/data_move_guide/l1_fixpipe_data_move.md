---
okftype: reference.concept
description: "Describes parameter loading from L1 to Fixpipe Buffer on 3510: contiguous and high-dimensional slicing prototypes of asc_copy_l12fb, independent address space selection, alignment, and capacity limits for quantization and ReLU parameters, and the PIPE_FIX synchronization requirement that parameter loading must be completed before L0C data is stored."
---
# L1 to Fixpipe Buffer Data Movement

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.245277+00:00 pushedAt=2026-11-16T02:10:55.326Z nl=2 -->

The Fixpipe Buffer stores the on-the-fly Vector quantization parameters and on-the-fly ReLU parameters used when L0C is moved out. NPU architecture version 3510 loads parameters from L1 through [asc_copy_l12fb](../cube_datamove/cube_compute_load/asc_copy_l12fb_arch_3510.md). The API runs on PIPE_FIX, takes effect only on AIC, and supports both contiguous and high-dimensional slicing modes.

## Parameter Address Space

Quantization parameters and ReLU parameters use independent Fixpipe Buffer address spaces, distinguished by the destination address `dst[19:16]`; `dst[15:0]` indicates the offset within the corresponding address space.

| Parameter Space | Address Selection | Destination Alignment | Capacity Limit |
| --- | --- | --- | --- |
| On-the-fly Vector quantization parameters | `dst[16]=0` | 128-byte | 4KB |
| On-the-fly ReLU parameters | `dst[16]=1` | 64-byte | 2KB |

The address offsets of the two types of parameters must be calculated separately. The end address of one space cannot be used as the start address of the other space.

## Contiguous Data Movement

In contiguous mode, `size` loads the first n bytes of parameters:

```c
__aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst,
                                      __cbuf__ void* src,
                                      uint32_t size)
```

- `size` is in bytes and must be 32-byte aligned. If not aligned, the actual amount of data moved is rounded down to a multiple of 32 bytes.
- When `size` is 0, no data movement is performed.
- The source L1 address must be 32-byte aligned, and the destination address must satisfy 64-byte or 128-byte alignment according to the parameter space.

## High-Dimensional Slicing Data Movement

The high-dimensional slicing mode can load parameters from multiple data blocks in L1 and preserve gaps on both the source and destination sides:

```c
__aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst,
                                      __cbuf__ void* src,
                                      uint16_t n_burst,
                                      uint16_t len_burst,
                                      uint16_t src_gap_size,
                                      uint16_t dst_gap_size)
```

- `len_burst` is in units of 64 bytes.
- `src_gap_size` and `dst_gap_size` indicate the gap from the end address of the previous data block to the start address of the next data block, in units of 32 bytes.
- In the on-the-fly quantization parameter scenario, `len_burst` and `dst_gap_size` must be multiples of 2; the on-the-fly ReLU parameter scenario does not have this even-number constraint.
- When `n_burst` is 0, no data movement is performed. For multi-block data movement, the destination stride must not exceed the capacity of the corresponding parameter space.

## Usage and Synchronization

- The total L1 capacity is 512 KB. The source address offset and high-dimensional slicing access span must not go out of bounds.
- When the Fixpipe Buffer goes out of bounds, the hardware may truncate the write and produce incorrect results. Do not rely on truncation protection.
- Parameter loading and L0C result movement both run on PIPE_FIX. Parameters must be ready before the L0C movement instruction that consumes them, and must not be overwritten while the corresponding movement is still executing.
- When parameters are written to L1 by another pipeline, a dependency on PIPE_FIX must be established first. When multiple `asc_copy_l12fb` instructions write overlapping regions, use [asc_sync_pipe](../sync/intra_core_sync/asc_sync_pipe.md) to ensure ordering.
- Quantization, activation mode, and the parameter layout in the Fixpipe Buffer are determined by the L0C movement API. Before loading, prepare parameters according to the functional combination of [asc_copy_l0c2gm](../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1](../cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md), or [asc_copy_l0c2ub](../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md).

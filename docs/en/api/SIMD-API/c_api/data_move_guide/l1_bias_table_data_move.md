---
okftype: reference.concept
description: "Describes bias movement from L1 to BiasTable on 3510: contiguous and high-dimensional slicing prototypes of asc_copy_l12bt, write conversion rules for source types such as half and bfloat16_t, conv_control constraints, and synchronization requirements for the L1 capacity limit of 512 KB and the BiasTable capacity limit of 4 KB."
---
# Data Movement from L1 to BiasTable

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.242278+00:00 pushedAt=2026-11-16T02:10:55.320Z nl=2 -->

The BiasTable Buffer stores the bias used by Mmad. On NPU architecture version 3510, [asc_copy_l12bt](../cube_datamove/cube_compute_load/asc_copy_l12bt_arch_3510.md) moves bias data from L1 to the BiasTable. The API runs on PIPE_MTE1, takes effect only on AIC, and supports both contiguous and high-dimensional slicing modes.

## Data Types and On-the-Fly Conversion

The source data types support `half`, `bfloat16_t`, `int32_t`, and `float`. The processing after writing to the BiasTable is as follows:

| Source Type | Write Method |
| --- | --- |
| `bfloat16_t` | Automatically converted to `float`. |
| `half` | When `conv_control` is 1, each element is converted to `float`; when it is 0, each element is expanded to 32 bits, with the upper 16 bits as placeholder data. |
| `int32_t`, `float` | The 32-bit data format is retained, and `conv_control` must be set to 0. |

## Contiguous Data Movement

The contiguous mode specifies a segment of contiguous bias data through `size`:

```c
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ <dtype>* src,
                                      uint32_t size)
```

- `size` is in bytes and must be 32-byte aligned. If not aligned, the actual amount of data moved is rounded down to a multiple of 32 bytes.
- When the source type is `float` or `int32_t`, `size / 32` must be an even number, meaning the actual amount of data moved must be 64-byte aligned.
- The contiguous mode does not pass `conv_control`. To control the `half` conversion method, select the high-dimensional slicing prototype and configure the data as one contiguous data block.

## High-Dimensional Slicing Data Movement

The high-dimensional slicing mode describes the bias layout by the number of data blocks, the length of each block, and the source and destination inter-block gaps:

```c
__aicore__ inline void asc_copy_l12bt(uint64_t dst,
                                      __cbuf__ <dtype>* src,
                                      uint16_t conv_control,
                                      uint16_t n_burst,
                                      uint16_t len_burst,
                                      uint16_t source_gap,
                                      uint16_t dst_gap)
```

- `len_burst` is in units of 32 bytes, with a value range of [1, 65535].
- `source_gap` and `dst_gap` indicate the gap from the end address of the previous data block to the start address of the next data block, in units of 32 bytes.
- When `n_burst` is 1, both gaps can be set to 0; when `n_burst` is 0, no data movement is performed.
- When the source type is `float` or `int32_t`, `len_burst` and `dst_gap` must be multiples of 2.
- `conv_control` is valid only for `half`; for other data types, set it to 0.

## Addresses, Capacity, and Synchronization

- The L1 source address must be 32-byte aligned, and the BiasTable destination address must be 64-byte aligned.
- The L1 capacity limit is 512 KB, and the BiasTable capacity limit is 4 KB. In high-dimensional slicing scenarios, the calculation of the address span must account for all inter-block gaps before the last data block.
- When L1 is written by another pipeline, establish a dependency on PIPE_MTE1 before `asc_copy_l12bt` reads it.
- After the BiasTable write is complete, establish a dependency from PIPE_MTE1 to PIPE_M before Mmad reads the bias through PIPE_M.
- When multiple data movements write overlapping BiasTable regions, use the same pipeline for synchronization to ensure that the previous instruction completes before overwriting.

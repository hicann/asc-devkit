---
okftype: reference.concept
description: "Describes the bidirectional GM and UB paths: contiguous, high-dimensional slicing, unaligned prototypes, and the parameter units and padding behavior of five-dimensional NDDMA from GM to UB, and provides PIPE_MTE2/MTE3 synchronization, UB source and destination address alignment, and access range calculation requirements."
---
# GM and UB Data Movement

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.237904+00:00 pushedAt=2026-11-16T02:10:55.314Z nl=2 -->

GM and UB data movement is the primary path for the Vector side to load inputs and write back results. GM-to-UB is executed by PIPE_MTE2, and UB-to-GM is executed by PIPE_MTE3; the related APIs take effect only on AIV. NPU architecture version 3510 supports contiguous, high-dimensional slicing, unaligned, and GM-to-UB NDDMA multi-dimensional data movement.

## General Description

| Direction | Copy Mode | API | Main Parameter Units |
| --- | --- | --- | --- |
| GM to UB | Contiguous | [asc_copy_gm2ub](../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md) | `size`: bytes |
| GM to UB | High-dimensional slicing | [asc_copy_gm2ub](../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md) | Block length, source/destination stride: bytes |
| GM to UB | Unaligned contiguous | [asc_copy_gm2ub_align](../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) | `size`: bytes |
| GM to UB | Unaligned high-dimensional slicing | [asc_copy_gm2ub_align](../vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) | Valid length, source/destination stride: bytes; left/right padding: elements |
| GM to UB | Padding, Transpose, Broadcast, or Slice with up to five dimensions | [asc_ndim_copy_gm2ub](../vector_datamove/asc_ndim_copy_gm2ub.md) | Loop size, stride, padding: elements |
| UB to GM | Contiguous | [asc_copy_ub2gm](../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) | `size`: bytes |
| UB to GM | High-dimensional slicing | [asc_copy_ub2gm](../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) | Block length, source/destination stride: bytes |
| UB to GM | Unaligned contiguous | [asc_copy_ub2gm_align](../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md) | `size`: bytes |
| UB to GM | Unaligned high-dimensional slicing | [asc_copy_ub2gm_align](../vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md) | Valid length, source/destination stride: bytes. |

The regular API expresses the valid length in bytes. It only needs to satisfy data type byte alignment, and the valid length does not need to be a multiple of 32 bytes. A non-32-byte valid length still causes aligned padding access on the UB side, and capacity calculation must include this range. When you need to explicitly control the on-chip padding layout, left/right padding from GM to UB, L2 cache policy, or a larger high-dimensional slicing parameter range, select the corresponding `*_align` high-dimensional slicing prototype. When you need to describe a three-dimensional or higher layout or implement multi-dimensional permutation, select NDDMA.

## asc_copy_gm2ub (Contiguous Data Movement from GM to UB)

This mode contiguously reads `size` bytes from GM and writes them to the UB in the original layout. The data format and content remain unchanged.

```c
__aicore__ inline void asc_copy_gm2ub(__ubuf__ void* dst,
                                      __gm__ void* src,
                                      uint32_t size)
```

- `dst` must be 32-byte aligned, and `src` must be 1-byte aligned.
- `size` is in bytes, with a value range of [1, $2^{21}-1$], and must satisfy the data type byte alignment requirement.
- `size` does not need to satisfy 32-byte alignment. Do not enlarge `size` to reach 32 bytes or read data outside the valid GM range. When the length is not 32-byte aligned, the hardware writes dummy data at the end of the destination UB to pad to the 32-byte boundary, so the UB must reserve space for the padded range.

## asc_copy_gm2ub (High-Dimensional Slicing Data Movement from GM to UB)

This mode moves `burst_count` data blocks, each containing `burst_len` bytes. `src_stride` and `dst_stride` respectively indicate the distance between the **start addresses** of adjacent source and destination data blocks, both in bytes.

```c
__aicore__ inline void asc_copy_gm2ub(__ubuf__ void* dst,
                                      __gm__ void* src,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t src_stride,
                                      uint16_t dst_stride)
```

- `dst` must be 32-byte aligned, and `src` must be 1-byte aligned.
- `burst_len` must satisfy data type byte alignment. When moving only one data block, both strides can be set to 0.
- When `dst_stride` equals `burst_len`, the layout is Compact. All valid data blocks are tightly packed, and only the overall tail is padded to the 32-byte boundary.
- When `dst_stride` is not equal to `burst_len`, the layout is Normal. `dst_stride` must be 32-byte aligned, and each data block is padded to the 32-byte boundary separately.
- The source GM span is calculated as the start offset of the last data block plus `burst_len`. The destination UB span must also include Compact overall padding or Normal per-block padding.

## asc_copy_gm2ub_align (Unaligned Data Movement from GM to UB)

This API pads data that is not 32-byte aligned to the 32-byte boundary when moving it into the UB, and supports 8-bit, 16-bit, and 32-bit data types. The contiguous mode only passes `size`. The high-dimensional slicing mode can also configure left/right padding, padding value source, L2 cache policy, and source/destination strides.

### Contiguous Mode

```c
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint32_t size)
```

When `size` is not an integer multiple of 32 bytes, the hardware pads the destination data to the 32-byte boundary, and the padding value is fixed to the first element of the data block. This contiguous prototype does not provide constant padding selection. If constant padding is required, use the high-dimensional slicing prototype, set `burst_count` to 1 and `enable_constant_pad` to `true`, and first configure the padding value through [asc_set_copy_pad_val](../vector_datamove/asc_set_copy_pad_val.md). The destination UB must reserve space for the padded range.

### High-Dimensional Slicing Mode

The high-dimensional slicing mode supports the following two destination layouts:

- **Compact mode:** `dst_stride` equals `burst_len`, and both left and right padding are 0. Multiple valid data blocks are tightly packed in the UB, and all data is padded to the 32-byte boundary only at the overall tail.
- **Normal mode:** `dst_stride` is not equal to `burst_len` and is an integer multiple of 32 bytes. Each data block is padded separately, and padding elements can be added on both sides through `left_padding_num` and `right_padding_num`.

When left/right padding is non-zero, `enable_constant_pad` does not take effect, and `asc_set_copy_pad_val` must be called first to configure the padding value. When both left and right padding are 0, `enable_constant_pad` set to `false` means padding with the first element of the data block, and set to `true` means using the constant configured by `asc_set_copy_pad_val`, in which case this configuration API must also be called first. When the same movement mode needs to be executed repeatedly, two levels of loops can be configured through [asc_set_gm2ub_loop_size](../vector_datamove/asc_set_gm2ub_loop_size.md), [asc_set_gm2ub_loop1_stride](../vector_datamove/asc_set_gm2ub_loop1_stride.md), and [asc_set_gm2ub_loop2_stride](../vector_datamove/asc_set_gm2ub_loop2_stride.md).

## asc_ndim_copy_gm2ub (Multi-Dimensional Data Movement from GM to UB)

NDDMA describes GM to UB data access through five levels of loops. Each dimension can configure the element count, source stride, destination stride, and left/right padding. Unused higher dimensions must still be filled with valid loop sizes as required by the API.

```c
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ <dtype>* dst,
                                           __gm__ <dtype>* src,
                                           uint32_t loop0_size,
                                           uint32_t loop1_size,
                                           uint32_t loop2_size,
                                           uint32_t loop3_size,
                                           uint32_t loop4_size,
                                           uint8_t loop0_lp_count,
                                           uint8_t loop0_rp_count,
                                           bool padding_mode,
                                           asc_load_l2_cache_mode l2_cache_mode)
```

- Before calling the main API, you must complete the source and destination stride configuration for each dimension through [asc_set_ndim_loop_stride](../vector_datamove/asc_set_ndim_loop_stride.md).
- Use [asc_set_ndim_pad_count](../vector_datamove/asc_set_ndim_pad_count.md) to set the left/right padding counts for dimensions 1 to 4. The padding count for dimension 0 is passed through the main API.
- Constant padding is set through [asc_set_ndim_pad_value](../vector_datamove/asc_set_ndim_pad_value.md). When `padding_mode` is `false`, the nearest boundary value is used for padding.
- The destination stride of each dimension must avoid write overlap between loops. The total address range accessible by one instruction cannot exceed 40 bits.
- When multiple cores may read the same GM block and the data may be updated by other cores, call [asc_ndim_copy_dci](../vector_datamove/asc_ndim_copy_dci.md) before the movement to flush the NDDMA DataCache.

NDDMA can implement multi-dimensional padding, transpose, broadcast, and slice through stride combinations, but it only supports the GM to UB direction.

## asc_copy_ub2gm (Contiguous Data Movement from UB to GM)

This mode contiguously reads `size` bytes from the UB and writes them to GM, with the data format and content unchanged.

```c
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst,
                                      __ubuf__ void* src,
                                      uint32_t size)
```

- `src` must be 32-byte aligned, and `dst` must be 1-byte aligned.
- `size` is in bytes, with a value range of [1, $2^{21}-1$], and must satisfy the data type byte alignment requirement.
- `size` does not need to be 32-byte aligned. The API writes only the valid bytes specified by `size` to GM and does not write other data within the aligned range in the UB to GM. However, the hardware reads from the UB up to the 32-byte boundary, so the source UB must reserve valid space for this range.

## asc_copy_ub2gm (High-Dimensional Slicing Data Movement from UB to GM)

This mode describes multiple valid data blocks through `burst_count` and `burst_len`. `src_stride` and `dst_stride` represent the byte distance between the start addresses of adjacent source and destination data blocks, respectively.

```c
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst,
                                      __ubuf__ void* src,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t dst_stride,
                                      uint16_t src_stride)
```

When `src_stride` equals `burst_len`, the layout is Compact, and the hardware pads the read only at the end of all valid data. When `src_stride` does not equal `burst_len`, the layout is Normal, `src_stride` must be 32-byte aligned, and the hardware pads the read for each data block separately. The padded portion is not written to GM but must be within the valid UB range. Setting `src_stride` to 0 for Broadcast is not recommended. To broadcast the same source data block to multiple GM locations, configure loops according to the API documentation using [asc_set_ub2gm_loop_size](../vector_datamove/asc_set_ub2gm_loop_size.md), [asc_set_ub2gm_loop1_stride](../vector_datamove/asc_set_ub2gm_loop1_stride.md), and [asc_set_ub2gm_loop2_stride](../vector_datamove/asc_set_ub2gm_loop2_stride.md).

## asc_copy_ub2gm_align (Unaligned Data Movement from UB to GM)

This API allows the valid data length to be a non-multiple of 32 bytes. When reading from the UB, the hardware pads dummy data to 32-byte alignment and discards the padded portion when writing to GM. Therefore, dummy data is not written to GM, and configuring a padding value is not supported.

The high-dimensional slicing mode supports two source layouts:

- **Compact mode:** `src_stride` equals `burst_len`. Source data blocks are tightly packed in the UB, and padding data is read only at the end of all valid data.
- **Normal mode:** `src_stride` does not equal `burst_len` and is a multiple of 32 bytes. Each data block is read separately to the 32-byte boundary.

Even though the padding data is not written to GM, the source UB must still reserve valid space for the aligned range read by the hardware.

## Synchronization and Boundary Check

- When PIPE_V reads from the UB after PIPE_MTE2 writes to it, establish an event dependency from PIPE_MTE2 to PIPE_V. When PIPE_MTE3 reads the result generated by PIPE_V, establish an event dependency from PIPE_V to PIPE_MTE3.
- When the destination addresses of multiple PIPE_MTE2 or PIPE_MTE3 data movement operations overlap, use [asc_sync_pipe](../sync/intra_core_sync/asc_sync_pipe.md) to ensure the execution order within the same pipeline.
- The GM access range is calculated based on the effective source stride. The UB access range must also include the maximum offset caused by alignment padding, Padding, and loop stride.
- The stride semantics of normal, unaligned, and NDDMA APIs differ, so the same set of unconverted parameters cannot be reused.

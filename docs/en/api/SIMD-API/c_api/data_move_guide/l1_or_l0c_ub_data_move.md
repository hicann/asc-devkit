---
okftype: reference.concept
description: "Describes the three on-chip unidirectional paths of 3510: UB to L1, L1 to UB, and L0C to UB. Provides the parameters, DataBlock units, dual-destination splitting constraints of asc_copy_ub2l1, asc_copy_l12ub, and asc_copy_l0c2ub, as well as the inter-core synchronization requirements between AI Cube and AI Vector."
---
# L1/L0C and UB Data Movement

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.252795+00:00 pushedAt=2026-11-16T02:10:55.339Z nl=2 -->

NPU architecture version 3510 supports three on-chip paths: L1 to UB, UB to L1, and L0C to UB, used to exchange inputs or intermediate results between the Cube side and the Vector side. All three paths are unidirectional and belong to PIPE_MTE1, PIPE_MTE3, and PIPE_FIX, respectively. NPU architecture version 3510 does not provide general data movement from UB to L0C.

## General Description

| Direction | Copy Mode | API | Pipeline | Execution Scope |
| --- | --- | --- | --- | --- |
| UB to L1 | Contiguous | [asc_copy_ub2l1](../vector_datamove/asc_copy_ub2l1.md) | PIPE_MTE3 | AIV |
| UB to L1 | High-dimensional slicing | [asc_copy_ub2l1](../vector_datamove/asc_copy_ub2l1.md) | PIPE_MTE3 | AIV |
| L1 to UB | Contiguous | [asc_copy_l12ub](../cube_datamove/cube_compute_store/asc_copy_l12ub.md) | PIPE_MTE1 | AIC |
| L1 to UB | High-dimensional slicing | [asc_copy_l12ub](../cube_datamove/cube_compute_store/asc_copy_l12ub.md) | PIPE_MTE1 | AIC |
| L0C to UB | Moves out matrix results, with optional quantization, activation, format conversion, or dual-destination writes. | [asc_copy_l0c2ub](../cube_datamove/cube_compute_store/asc_copy_l0c2ub.md) | PIPE_FIX | AIC |

## asc_copy_ub2l1 (UB to L1 Contiguous Data Movement)

In contiguous mode, data is moved from UB to L1 by the number of bytes specified by `size`, with the data format and content unchanged.

```c
__aicore__ inline void asc_copy_ub2l1(__cbuf__ void* dst,
                                      __ubuf__ void* src,
                                      uint32_t size)
```

- Both the source UB address and the destination L1 address must be 32-byte aligned.
- `size` is in bytes, with a value range of [32, 4095×32], and must be an integer multiple of 32.
- When `size` is not aligned, the hardware uses integer division (`size / 32`) to determine the number of complete blocks to move, and the remaining bytes are not moved.

## asc_copy_ub2l1 (UB to L1 High-Dimensional Slicing Data Movement)

High-dimensional slicing mode describes non-contiguous layouts through the number of data blocks, the length of each block, and the source and destination inter-block gaps.

```c
__aicore__ inline void asc_copy_ub2l1(__cbuf__ void* dst,
                                      __ubuf__ void* src,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t src_gap,
                                      uint16_t dst_gap)
```

- `burst_len` is in units of 32-byte DataBlocks.
- `src_gap` and `dst_gap` represent the gap between the end address of the previous data block and the start address of the next data block, also in units of DataBlocks.
- When `burst_count` is 1, both gaps can be set to 0. For multi-block data movement, the calculation of the maximum access span for both the source and destination must account for the inter-block gaps.

## asc_copy_l12ub (L1 to UB Data Movement)

This API is issued from the AIC and writes L1 data to the UB of the specified AIV. The high-dimensional slicing layout is described by `burst_count`, `burst_len`, `src_gap`, and `dst_gap`. In contiguous scenarios, a single data block can be configured, or the inter-block gaps can be set to 0.

```c
__aicore__ inline void asc_copy_l12ub(__ubuf__ void* dst_addr,
                                      __cbuf__ void* src_addr,
                                      int8_t sub_blockid,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t src_gap,
                                      uint16_t dst_gap)
```

- Both `src_addr` and `dst_addr` must be 32-byte aligned.
- When `sub_blockid` is 0, data is written to the UB of AIV0; when it is 1, data is written to the UB of AIV1.
- `burst_len`, `src_gap`, and `dst_gap` are all in units of 32 bytes. `src_gap` and `dst_gap` represent the inter-block gaps, not the distance between the start addresses of adjacent blocks.
- The L1 capacity upper limit is 512 KB; the nominal UB capacity is 256 KB, but the actual available space must exclude the SIMD VF stack, reserved space, and the SIMT Data Cache if enabled.

## asc_copy_l0c2ub (L0C to UB Data Movement with On-the-Fly Processing)

This API writes Cube computation results directly to the UB of one or two AIVs, and can combine type conversion, quantization, activation, and format conversion in PIPE_FIX.

```c
__aicore__ inline void asc_copy_l0c2ub(__ubuf__ <dst_dtype>* dst,
                                       __cc__ <src_dtype>* src,
                                       uint16_t n_size,
                                       uint16_t m_size,
                                       uint32_t dst_stride,
                                       uint16_t src_stride,
                                       int8_t sub_blockid,
                                       asc_dual_dst_mode dual_dst_ctrl,
                                       asc_unit_flag_mode unit_flag_mode,
                                       asc_quant_mode quant_pre_mode,
                                       asc_relu_pre_mode relu_pre_mode,
                                       bool enable_channel_split,
                                       bool enable_nz2nd,
                                       bool enable_nz2dn,
                                       bool enable_clip_relu_pre)
```

### Destination AIV Selection

- In single-destination mode, `sub_blockid` selects AIV0 or AIV1, and the complete matrix is written to the corresponding UB.
- `asc_dual_dst_mode::DUAL_DST_SPLIT_M` splits the matrix into two halves along the M dimension and writes them to two UBs respectively. M must be a multiple of 2.
- `asc_dual_dst_mode::DUAL_DST_SPLIT_N` splits along the N dimension. N must be a multiple of 32.
- Dual-destination mode supports only ordinary Nz2Nz or Nz2ND scenarios and cannot be arbitrarily combined with other on-the-fly processing capabilities.

### Format, Quantization, and Activation

| Function | Main API Parameters | Pre-Configuration |
| --- | --- | --- |
| Nz2ND | `enable_nz2nd` | [asc_set_l0c_copy_nz_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md) |
| Nz2DN | `enable_nz2dn` | `asc_set_l0c_copy_nz_para`, [asc_set_l0c_copy_channel_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md) |
| Scalar quantization | `quant_pre_mode` | [asc_set_l0c_copy_prequant](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md) |
| Tensor quantization | `quant_pre_mode` | [asc_set_l0c_copy_config](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md) |
| ReLU/Leaky ReLU | `relu_pre_mode`, `enable_clip_relu_pre` | [asc_set_l0c_copy_relu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md), [asc_set_l0c_copy_lrelu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md) |

`src_dtype` is `int32_t` or `float`, and the destination type is determined by the quantization or Cast mode. The source and destination data types must match `quant_pre_mode`, and quantization coefficients cannot be INF, NAN, or denormalized numbers.

### Stride and Bounds

- The source L0C address must be 64-byte aligned, and the destination UB address must be 32-byte aligned.
- `src_stride` is in units of 64 bytes and indicates the distance between the start addresses of adjacent Z layouts in the source Nz matrix.
- `dst_stride` is in units of elements; for Nz output it indicates the spacing between adjacent Z layouts, and for ND/DN output it indicates the number of elements in a row, whose corresponding byte length must be 32-byte aligned.
- The L0C and UB access ranges must not exceed 256 KB and the actually allocated UB space, respectively. In dual-destination mode, the destination ranges of the two AIVs must be checked separately.

## Synchronization Requirements

- After an AIV writes UB to L1 through PIPE_MTE3, inter-core synchronization from AIV to AIC must be completed and subsequent pipeline dependencies must be established before the AIC reads L1. When the AIC depends on all AIVs in the same group, use mode 2 [asc_sync_block_arrive](../sync/inter_core_sync/asc_sync_block_arrive.md) and [asc_sync_block_wait](../sync/inter_core_sync/asc_sync_block_wait.md), where each AIV sends a notification and the AIC waits; when depending on only a single AIV, use mode 4 [asc_sync_intra_arrive](../sync/inter_core_sync/asc_sync_intra_arrive.md) and [asc_sync_intra_wait](../sync/inter_core_sync/asc_sync_intra_wait.md).
- After the AIC writes L1 to UB through PIPE_MTE1 or writes L0C to UB through PIPE_FIX, synchronization from the AIC to the target AIV must be completed before the AIV reads UB. When all AIVs consume the data, use mode 2, where the AIC sends a notification and each AIV waits; when only a specified AIV consumes the data, use mode 4.
- L0C is generated by PIPE_M, and synchronization is required before PIPE_FIX moves it out; when conditions are met, UnitFlag can also be used to enable Cube computation and data movement out to run in parallel on a per-fractal basis.
- When multiple instructions write to the same L1 or UB range, they must be serialized according to the requirements in [Synchronization Control](../sync/overview.md#synchronization-control-overview) and cannot rely solely on source code order.

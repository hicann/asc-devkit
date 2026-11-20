---
okftype: reference.concept
description: "Describes the GM and L1/L0 paths of 3510: contiguous, high-dimensional slicing, two-dimensional fractal, ND2Nz, DN2Nz, and unaligned movement from GM to L1, as well as combined quantized activation format conversion movement from L0C to GM, and provides the parameter units, alignment, and L1 space requirements of each prototype."
---
# GM and L1/L0 Data Movement

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.232893+00:00 pushedAt=2026-11-16T02:10:55.307Z nl=2 -->

NPU architecture version 3510 provides two direct paths: GM to L1 Buffer and L0C Buffer to GM. When matrix inputs enter L0A Buffer/L0B Buffer from GM, they must first be moved to L1 Buffer through PIPE_MTE2 and then loaded into L0A Buffer/L0B Buffer through PIPE_MTE1. Cube computation results can be moved directly back to GM from L0C Buffer through PIPE_FIX. NPU architecture version 3510 does not provide direct movement from GM to L0A Buffer/L0B Buffer or from L1 Buffer to GM.

## General Description

| Direction | Copy Mode | API | Pipeline |
| --- | --- | --- | --- |
| GM to L1 | Contiguous copy, supports channel padding. | [asc_copy_gm2l1](../cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md) | PIPE_MTE2 |
| GM to L1 | High-dimensional slicing copy, supports channel padding. | [asc_copy_gm2l1](../cube_datamove/cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md) | PIPE_MTE2 |
| GM to L1 | Two-dimensional fractal matrix copy | [asc_copy_gm2l1](../cube_datamove/cube_compute_load/asc_copy_gm2l1_2d_arch_3510.md) | PIPE_MTE2 |
| GM to L1 | ND to Nz format conversion | [asc_copy_gm2l1_nd2nz](../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md) | PIPE_MTE2 |
| GM to L1 | DN to Nz format conversion | [asc_copy_gm2l1_dn2nz](../cube_datamove/cube_compute_load/asc_copy_gm2l1_dn2nz.md) | PIPE_MTE2 |
| GM to L1 | Unaligned copy and padding | [asc_copy_gm2l1_align](../cube_datamove/cube_compute_load/asc_copy_gm2l1_align.md) | PIPE_MTE2 |
| L0C to GM | Matrix result move-out with optional quantization, activation, and format conversion. | [asc_copy_l0c2gm](../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md) | PIPE_FIX |

## asc_copy_gm2l1 (GM to L1 Contiguous or High-Dimensional Slicing Data Movement)

This API performs data movement according to the number of data blocks specified by `n_burst`. Each data block contains `len_burst` 32-byte units. `src_stride` and `dst_stride` indicate the distance between the start addresses of adjacent source and destination data blocks, also in 32-byte units. When moving only one contiguous block of data, set `n_burst` to 1 and both strides to 0.

```c
__aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst,
                                      __gm__ void* src,
                                      uint32_t n_burst,
                                      uint32_t len_burst,
                                      asc_channel_pad_mode pad_mode,
                                      uint64_t src_stride,
                                      uint32_t dst_stride)
```

- `dst` must be 32-byte aligned, and `src` must be 1-byte aligned.
- `pad_mode` is used to select no processing, insert padding by channel, or remove padding. When using the insert padding mode, first configure the padding value through [asc_set_gm2l1_padding](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_padding.md). This configuration API does not set the padding mode.
- When inserting padding, `len_burst` of each data block must be set as required by the API. The actual amount of data written to L1 may differ from the amount read from GM.
- When removing padding, the actual amount written to the destination is determined by the removal mode. When calculating L1 space, do not use only the amount read from the source.

## asc_copy_gm2l1 (GM to L1 2D Fractal Matrix Data Movement)

In 2D mode, this API selects complete fractals from the GM matrix according to the row/column start positions and strides, and writes them to L1. Each fractal is fixed at 512 bytes, and no format conversion is performed during the data movement.

```c
__aicore__ inline void asc_copy_gm2l1(__cbuf__ <dtype>* dst,
                                      __gm__ <dtype>* src,
                                      uint32_t m_start_position,
                                      uint32_t k_start_position,
                                      uint16_t dst_stride,
                                      uint16_t m_step,
                                      uint16_t k_step,
                                      asc_load_l2_cache_mode l2_cache_mode)
```

- `m_start_position` and `m_step` are in units of 16 elements, and `k_start_position` and `k_step` are in units of 32 bytes.
- `dst_stride` is in units of 512 bytes and indicates the distance between the start addresses of adjacent fractals in the column direction of the destination matrix.
- When `m_step` or `k_step` is 0, no data movement is performed; the API does not support moving only part of the elements within a fractal.
- When the boundary is less than one fractal, a complete fractal should be prepared on the GM side according to the subsequent Cube computation requirements.

## asc_copy_gm2l1_nd2nz (GM to L1 On-the-Fly ND2Nz Data Movement)

This API reads an ND-layout matrix from GM and converts it to Nz fractal layout when writing to L1. The source matrix can contain multiple ND matrices. `src_d_value` describes the distance between the start addresses of adjacent rows in the same matrix, and `src_nd_matrix_stride` describes the distance between the start addresses of adjacent ND matrices.

```c
__aicore__ inline void asc_copy_gm2l1_nd2nz(__cbuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint64_t src_d_value,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint16_t n_value,
                                            uint32_t d_value,
                                            uint64_t src_nd_matrix_stride,
                                            bool enable_small_c0)
```

- Before calling, use [asc_set_gm2l1_nz_para](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_nz_para.md) to set the destination Nz matrix stride and the number of ND matrices.
- `src_d_value` and `src_nd_matrix_stride` are in bytes, and `n_value` and `d_value` are in elements.
- When the valid data length in each row is not a multiple of 32 bytes, the standard mode pads the destination matrix with zeros to the 32-byte boundary.
- SmallC0 mode can be enabled only when `d_value` is not greater than 4; in this case, padding is performed at a granularity of 4 elements.

## asc_copy_gm2l1_dn2nz (GM to L1 On-the-Fly DN2Nz Data Movement)

This API converts the DN layout in GM to the Nz layout in L1 during the data movement, and is suitable for data that is contiguous in the depth direction. The parameter form is similar to that of the ND2Nz API, but `n_value` indicates the number of columns in the DN matrix, and `d_value` indicates the number of rows in the DN matrix.

- Before calling, also use `asc_set_gm2l1_nz_para` to set the destination Nz matrix stride and the number of DN matrices.
- When `src_nd_matrix_stride` is 0 and the number of matrices is greater than 1, it indicates that the first DN matrix is read repeatedly.
- The enabling conditions, address alignment, and L1 capacity requirements for SmallC0 are consistent with the API documentation.

## asc_copy_gm2l1_align (GM to L1 Unaligned Data Movement)

This API supports data whose valid block length is not an integer multiple of 32 bytes, and can perform padding in the destination L1. The high-dimensional slicing mode describes the data movement through the number of data blocks, valid byte count, left/right padding, padding source, L2 Cache policy, and source/destination strides.

```c
__aicore__ inline void asc_copy_gm2l1_align(__cbuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            uint8_t left_padding_count,
                                            uint8_t right_padding_count,
                                            bool enable_data_select,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint64_t src_stride,
                                            uint32_t dst_stride)
```

- **Compact mode:** `dst_stride` is equal to `burst_len`, and the left/right padding is 0. Multiple data blocks are tightly arranged in L1, and padding to the 32-byte boundary is performed only at the overall end.
- **Normal mode:** `dst_stride` is not equal to `burst_len` and is an integer multiple of 32 bytes, and each data block is padded separately. This alignment constraint still takes effect when `burst_count` is 1.
- **Padding value source:** When both left and right padding are 0, `enable_data_select` set to `false` indicates using the first element of each data block for padding, and set to `true` indicates using the constant preconfigured by [asc_set_gm2l1_pad](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_pad.md). When `enable_data_select` is `true`, the padding value must be configured first even if both left and right padding are 0.
- **Left/right padding mode:** When `left_padding_count` or `right_padding_count` is non-zero, the left and right counts are in elements, `enable_data_select` does not take effect, and the hardware forcibly uses constant padding. In this case, `asc_set_gm2l1_pad` must be called first to configure the padding value.
- **Loop padding mode:** Two-level loops are configured through [asc_set_gm2l1_loop_size](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop_size.md), [asc_set_gm2l1_loop1_stride](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop1_stride.md), and [asc_set_gm2l1_loop2_stride](../cube_datamove/cube_load_aux_config/asc_set_gm2l1_loop2_stride.md). It cannot be enabled together with left/right padding.

`burst_len`, `src_stride`, and `dst_stride` are all in bytes; `src` requires 1-byte alignment, and `dst` requires 32-byte alignment. In loop padding mode, the loop1 and loop2 destination strides must also be 32-byte aligned, and this still takes effect when the corresponding loop count is 1. The destination L1 space must include valid data, left/right padding, dummy data padded to the 32-byte boundary, and the maximum offset generated by `dst_stride` and the loop destination strides.

## asc_copy_l0c2gm (L0C to GM On-the-Fly Processing Data Movement)

This API moves Cube computation results from L0C to GM and can combine the following capabilities in PIPE_FIX:

- Output Nz layout, or perform Nz2ND or Nz2DN format conversion.
- Convert `int32_t` or `float` results to 4-bit, 8-bit, 16-bit, or 32-bit destination types.
- Perform scalar or tensor quantization.
- Executes activation modes such as Normal, Scalar, Vector, or Clip ReLU.
- Performs channel splitting when data type and layout constraints are satisfied.
- Establishes fractal-granularity pipeline parallelism with Cube computation through UnitFlag.

Configure the following states based on the functional combination before calling:

| Function | Configuration API |
| --- | --- |
| Nz2ND/Nz2DN | [asc_set_l0c_copy_nz_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md) |
| Nz2DN channel parameters | [asc_set_l0c_copy_channel_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md) |
| Scalar quantization | [asc_set_l0c_copy_prequant](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md) |
| Tensor quantization parameter address | [asc_set_l0c_copy_config](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md) |
| ReLU/Leaky ReLU parameters | [asc_set_l0c_copy_relu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md), [asc_set_l0c_copy_lrelu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md) |

`src` requires 64-byte alignment, and `dst` requires 1-byte alignment. `n_size`, `m_size`, `dst_stride`, and the source and destination data types must match the format conversion, quantization, and activation combination; the destination GM space should be calculated based on the converted data type and layout.

## Two-Stage Data Movement from GM to L0A/L0B

Matrix inputs for NPU architecture version 3510 need to be prepared in the following order:

1. Use the GM-to-L1 APIs in this chapter to move input data or fractals to L1.
2. Establish data dependency between PIPE_MTE2 and PIPE_MTE1.
3. Use the two-dimensional, three-dimensional, transpose, or MX APIs in [L1 and L0 Data Movement](l1_l0_data_move.md) to load L1 data to L0A/L0B.
4. Initiate Cube computation after establishing dependency between PIPE_MTE1 and PIPE_M.

The stride units of the two-stage data movement are different. GM-to-L1 high-dimensional slicing describes block length and start address spacing in 32-byte units, while L1-to-L0 two-dimensional data movement describes matrix positions using a mixture of 16 elements, 32 bytes, and 512-byte fractals, so parameters cannot be directly reused.

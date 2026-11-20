---
okftype: reference.concept
description: "Describes directed movement between L1 and L0 on 3510: two-dimensional fractal with transpose, three-dimensional img2col, Repeat transpose, and MX quantization coefficient loading from L1 to L0A/L0B, as well as accompanying processing movement from L0C to L1, and provides fractal shapes, alignment, and capacity constraints."
---
# L1 and L0 Data Movement

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.247277+00:00 pushedAt=2026-11-16T02:10:55.332Z nl=2 -->

L1 to L0A/L0B is used to load the left and right matrices for Cube computation and is executed by PIPE_MTE1. The Cube computation result is located in L0C and can be moved to L1 by PIPE_FIX. The data path of NPU architecture version 3510 is directional and does not provide general-purpose copies from L0A/L0B to L1 or from L1 to L0C.

## General Description

| Direction | Copy Mode | API |
| --- | --- | --- |
| L1 to L0A | Two-dimensional fractal data movement | [asc_copy_l12l0a](../cube_datamove/cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md) |
| L1 to L0A | Two-dimensional fractal data movement with transpose | [asc_copy_l12l0a_transpose](../cube_datamove/cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md) |
| L1 to L0A | Three-dimensional img2col data movement | [asc_copy_l12l0a](../cube_datamove/cube_compute_load/asc_copy_l12l0a_3d_arch_3510.md) |
| L1 to L0A_MX | MX left matrix quantization coefficient movement | [asc_copy_l12l0a_mx](../cube_datamove/cube_compute_load/asc_copy_l12l0a_mx.md) |
| L1 to L0B | Two-dimensional fractal data movement | [asc_copy_l12l0b](../cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md) |
| L1 to L0B | Two-dimensional fractal data movement with transpose | [asc_copy_l12l0b_transpose](../cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md) |
| L1 to L0B | Three-dimensional img2col data movement | [asc_copy_l12l0b](../cube_datamove/cube_compute_load/asc_copy_l12l0b_3d_arch_3510.md) |
| L1 to L0B | Two-dimensional fractal transpose based on Repeat and fractal interval | [asc_copy_l12l0b_trans](../cube_datamove/cube_compute_load/asc_copy_l12l0b_trans_arch_3510.md) |
| L1 to L0B_MX | MX right matrix quantization coefficient movement | [asc_copy_l12l0b_mx](../cube_datamove/cube_compute_load/asc_copy_l12l0b_mx.md) |
| L0C to L1 | Matrix result move-out with optional quantization, activation, and format conversion | [asc_copy_l0c2l1](../cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md) |

## L1 to L0A Two-Dimensional Fractal Matrix Movement

[asc_copy_l12l0a](../cube_datamove/cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md) loads a two-dimensional matrix from L1 to L0A according to the source matrix row and column start positions, the number of movement fractals, and the source/destination fractal strides. `asc_copy_l12l0a_transpose` uses the same parameter format and transposes the fractal matrix during data movement.

```c
__aicore__ inline void asc_copy_l12l0a(__ca__ <dtype>* dst,
                                       __cbuf__ <dtype>* src,
                                       uint16_t m_start_position,
                                       uint16_t k_start_position,
                                       uint8_t m_step,
                                       uint8_t k_step,
                                       int16_t src_stride,
                                       uint16_t dst_stride)
```

- `src` must be 32-byte aligned, and `dst` must be 512-byte aligned.
- `m_start_position` and `m_step` are in units of 16 elements; `k_start_position` is in units of 32 bytes.
- `src_stride` and `dst_stride` are both in units of 512-byte fractals.
- L0A fractal shapes differ by data bit width: b4 is 16×64, b8 is 16×32, b16 is 16×16, and b32 is 16×8. Each fractal occupies 512 bytes.
- When `m_step` or `k_step` is 0, no data movement is performed. In transpose mode, stride multiple constraints corresponding to different data bit widths must also be satisfied.

## L1 to L0B Two-Dimensional Fractal Matrix Movement

The parameter semantics of [asc_copy_l12l0b](../cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md) and `asc_copy_l12l0b_transpose` are the same as those of L0A two-dimensional data movement, except that the destination is L0B. The L0B fractal shapes for different data bit widths are: 64×16 for b4, 32×16 for b8, 16×16 for b16, and 8×16 for b32.

The minimum granularity for 2D data movement is one complete 512-byte fractal. When the matrix boundary is smaller than one fractal, a complete fractal must be prepared in L1 and padded with invalid elements as required by Cube computation. The destination range of L0B must not exceed 64 KB.

## 3D img2col Data Movement from L1 to L0A

In 3D mode, L0A performs Image to Column expansion on the NC1HWC0 Feature Map in L1, selects fractals based on the destination matrix start point and range, and writes them to L0A. The main parameters include:

- `k_extension` and `m_extension`: Data movement length in the width and height directions of the destination matrix.
- `k_start_pt` and `m_start_pt`: Start point in the width and height directions of the destination matrix.
- `stride_w` and `stride_h`: Sliding stride of the convolution kernel.
- `filter_w`, `filter_h`, and dilation coefficients: Describe the convolution window.
- `channel_size`: Number of channels in the source Feature Map.

Before calling the main API, complete the following configurations:

| Configuration Item | API |
| --- | --- |
| Feature Map width, height, and Padding on all sides | [asc_set_l13d_fmatrix](../cube_datamove/cube_load_aux_config/asc_set_l13d_fmatrix.md) |
| Padding value and Padding mode | [asc_set_l12l0a_3d_padding](../cube_datamove/cube_load_aux_config/asc_set_l12l0a_3d_padding.md) |
| Destination stride, Repeat direction, Repeat count, and Repeat stride | [asc_set_l13d_rpt](../cube_datamove/cube_load_aux_config/asc_set_l13d_rpt.md) |

The valid range of `stride_w` and `stride_h` is [1, 63]. For the conditions under which the data movement length, convolution kernel size, or channel count makes the API a no-op, as well as the channel remainder constraints for each data bit width, refer to the API documentation.

## 3D img2col Data Movement from L1 to L0B

The 3D mode of L0B also performs img2col expansion, but the hardware automatically transposes the data when writing to L0B. The `transpose` parameter in the main API does not change this behavior. Before the call, configure the register group corresponding to the right matrix through the following APIs:

- [asc_set_l13d_fmatrix_b](../cube_datamove/cube_load_aux_config/asc_set_l13d_fmatrix_b.md): Feature Map attributes.
- [asc_set_l12l0b_3d_padding](../cube_datamove/cube_load_aux_config/asc_set_l12l0b_3d_padding.md): Padding value and mode.
- [asc_set_l13d_rpt_b](../cube_datamove/cube_load_aux_config/asc_set_l13d_rpt_b.md): Destination stride and repeat parameters.

The 3D APIs for L0A and L0B can also be used for ordinary 2D matrix loading, but the addresses, data movement ranges, and configuration registers must still be calculated according to the 3D API rules.

## 2D Fractal Transpose Data Movement from L1 to L0B

[asc_copy_l12l0b_trans](../cube_datamove/cube_compute_load/asc_copy_l12l0b_trans_arch_3510.md) processes one or more 512-byte fractals according to the `repeat` count, and uses `src_stride`, `dst_gap`, `src_frac_gap`, and `dst_frac_gap` to control the fractal layout between repeats and within the same repeat.

```c
__aicore__ inline void asc_copy_l12l0b_trans(__cb__ <dtype>* dst,
                                             __cbuf__ <dtype>* src,
                                             uint16_t index_id,
                                             uint8_t repeat,
                                             uint16_t src_stride,
                                             uint16_t dst_gap,
                                             uint16_t dst_frac_gap,
                                             uint16_t src_frac_gap)
```

- `src` must be 32-byte aligned, and `dst` must be 512-byte aligned.
- All stride and gap parameters are in units of 512-byte fractals. The effective hardware stride of `dst_gap`, `dst_frac_gap`, and `src_frac_gap` is the passed value plus 1.
- `index_id` specifies the starting fractal index of the source matrix. The number of fractals processed per repeat and the square matrix shape vary with the data bit width, and should be calculated for the corresponding data type according to the API documentation.
- Destination fractals must not overlap. When `repeat` is 0, no data movement is performed.

## MX Quantization Coefficient Data Movement

MX Cube computation requires loading the main matrix and the corresponding quantization coefficients into L0A/L0A_MX or L0B/L0B_MX separately. The quantization coefficient data movement APIs include:

- [asc_copy_l12l0a_mx](../cube_datamove/cube_compute_load/asc_copy_l12l0a_mx.md): The quantization coefficient fractal is fixed at 16×2, corresponding to the address mapping of one 16×32 data fractal in L0A.
- [asc_copy_l12l0b_mx](../cube_datamove/cube_compute_load/asc_copy_l12l0b_mx.md): The quantization coefficient fractal is fixed at 2×16, corresponding to the address mapping of one 32×16 data fractal in L0B.

For both APIs, the source L1 address and the destination MX buffer address must be 32-byte aligned, and the minimum data movement granularity is one 32-byte quantization coefficient fractal. The capacity of L0A_MX and L0B_MX is 4 KB each. When `x_step` or `y_step` is 0, the API is a no-op. The destination address and stride must establish a correct mapping with the main matrix fractals.

## asc_copy_l0c2l1 (L0C to L1 Data Movement with On-the-Fly Processing)

This API moves an `int32_t` or `float` matrix result out of L0C and, when writing to L1, can combine type conversion, scalar/tensor quantization, ReLU or Leaky ReLU, Nz2ND/Nz2DN, channel splitting, and UnitFlag.

Before enabling the corresponding capabilities, configure the state through the following APIs:

| Function | Configuration API |
| --- | --- |
| Nz2ND/Nz2DN | [asc_set_l0c_copy_nz_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_nz_para.md) |
| Nz2DN channel parameters | [asc_set_l0c_copy_channel_para](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_channel_para.md) |
| Scalar quantization | [asc_set_l0c_copy_prequant](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_prequant.md) |
| Tensor quantization parameter address | [asc_set_l0c_copy_config](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md) |
| ReLU/Leaky ReLU parameters | [asc_set_l0c_copy_relu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md), [asc_set_l0c_copy_lrelu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md) |

`src` must be 64-byte aligned, and `dst` must be 32-byte aligned. The alignment of `n_size`, `m_size`, and `dst_stride` is jointly determined by the destination data bit width and the output format. The source and destination data types must match `quant_pre_mode`.

## Synchronization and Boundary Check

- After L1 is written by PIPE_MTE2 or another pipeline, a dependency must be established before PIPE_MTE1 reads it. After L0A/L0B are written, a dependency must also be established before PIPE_M starts Cube computation.
- After PIPE_M writes L0C, synchronization is required before PIPE_FIX moves the result out, or UnitFlag can be used to establish fractal-granularity parallelism when the API constraints are met.
- The L1 capacity limit is 512 KB, L0A/L0B are 64 KB each, and L0C is 256 KB. In both 2D and 3D modes, the complete boundary fractal must be included in the access range.
- The Feature Map, Padding, Repeat, quantization, and format conversion states set by the configuration APIs affect subsequent data movement. Recheck them against the current task before reuse.

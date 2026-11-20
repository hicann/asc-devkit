---
okftype: reference.api
description: "Deprecated API page: under the 2201 architecture, asc_copy_l12l0b_sync moves a 512-byte matrix from L1 Buffer to L0B Buffer, including two forms: high-dimensional slicing 2D transfer by fractal numbering and 3D img2col transfer. For 2D transpose, use asc_copy_l12l0b_trans instead. It is recommended to use the asynchronous interface with asc_sync instead."
tag:
  - roles: [data_movement]
  - operations: [img2col, load]
  - traits: [high_dimensional]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_arch_2201.md
    kind: replaced_by
    condition: "It is recommended to use the corresponding asynchronous interface and explicitly call asc_sync"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_trans_arch_2201.md
    kind: related
    condition: "Use this interface for 2D format transpose scenarios"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Space and alignment requirements are described per their storage unit"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header is marked with ASC_DEPRECATED(9.2.0) before the asc_copy_l12l0b_sync declaration. The replacement path is the non-synchronous asc_copy_l12l0b plus asc_sync; the interface is not removed."
---
# asc_copy_l12l0b_sync (NPU Architecture Version 2201, Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:24:42.429Z pushedAt=2026-11-18T03:12:16.437Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is: `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_copy_l12l0b_sync` interface is deprecated. Use the [corresponding non-synchronous interface](../cube_datamove/cube_compute_load/asc_copy_l12l0b_arch_2201.md) and the synchronous interface [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Used to transfer a 512B matrix stored in the L1 Buffer to the L0B Buffer. Includes 2D format transfer and 3D format transfer.

## Prototype

- Synchronous high-dimensional split transfer in 2D format

    ```cpp
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ half* dst, __cbuf__ half* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ float* dst, __cbuf__ float* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    ```

- Synchronous high-dimensional split transfer in 3D format

    ```cpp
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ half* dst, __cbuf__ half* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    __aicore__ inline void asc_copy_l12l0b_sync(__cb__ float* dst, __cbuf__ float* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt, uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h, uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose, bool f_matrix_ctrl, uint16_t channel_size)
    ```

## Parameters

**Table 1** 2D format parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Destination L0B Buffer address. |
| src | Input | Source L1 Buffer address. |
| start_index | Input | Fractal matrix ID, indicating that the transfer start position is the Nth fractal in the source operand (0 indicates the first fractal matrix in the source operand). Value range: [0, 65535]. Unit: 512 bytes. |
| repeat | Input | Number of iterations. Each iteration can process 512B of data. Value range: [1, 255]. |
| src_stride | Input | The interval between the start addresses of the previous fractal and the next fractal in the source operand between adjacent iterations. Value range: [0, 65535]. Unit: 512 bytes. |
| dst_gap | Input | The interval between adjacent contiguous data blocks in the destination operand (the interval between the tail of the previous data block and the head of the next data block). Value range: [0, 65535]. Unit: 512 bytes. |

**Table 2** 3D format parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Destination L0B Buffer address. |
| src | Input | Source L1 Buffer address. |
| k_extension | Input | The transfer length of this instruction in the width dimension of the destination operand. If the rightmost fractal is not covered, it must be a multiple of 16 for the half type and a multiple of 32 for the int8_t/uint8_t type; if the rightmost fractal is covered, there is no multiple requirement. Value range: [1, 65535]. |
| m_extension | Input | The transfer length of this instruction in the height dimension of the destination operand. If the bottommost fractal is not covered, it must be a multiple of 16 for the half/int8_t/uint8_t type; if the bottommost fractal is covered, there is no multiple requirement. Value range: [1, 65535]. |
| k_start_pt | Input | The start point of this instruction in the width dimension of the destination operand. It must be a multiple of 16 for the half type and a multiple of 32 for the int8_t/uint8_t type. Value range: [0, 65535] |
| m_start_pt | Input | The start point of this instruction in the height dimension of the destination operand. If the bottommost fractal is not covered, it must be a multiple of 16 for half/int8_t/uint8_t; if the bottommost fractal is covered, there is no multiple requirement. Value range: [0, 65535].|
| stride_w | Input | The step size of the convolution kernel sliding in the width dimension of the source operand. Value range: [1, 63]. |
| stride_h | Input | The step size of the convolution kernel sliding in the height dimension of the source operand. Value range: [1, 63]. |
| filter_w | Input | Convolution kernel width. Value range: [1, 255]. |
| filter_h | Input | Convolution kernel height. Value range: [1, 255]. |
| dilation_filter_w | Input | Dilation factor of the convolution kernel width. Value range: [1, 255]. |
| dilation_filter_h | Input | Dilation factor of the convolution kernel height. Value range: [1, 255]. |
| filter_size_w | Input | Whether to increase the convolution kernel width by 256 elements on the basis of filter_w. true indicates increase; false indicates no increase. |
| filter_size_h | Input | Whether to increase the convolution kernel height by 256 elements on the basis of filter_h. true indicates increase; false indicates no increase. |
| transpose | Input | Whether to enable the transpose function, which transposes the entire target matrix. It is valid only when the source operand is of the half type. true indicates enable; false indicates disable. |
| f_matrix_ctrl | Input | Indicates the attribute description of whether the asc_copy_l12l0b instruction obtains the FeatureMap from the left matrix or the right matrix. Currently only false is supported. |
| channel_size | Input | The number of channels of the source operand. Value range: [1, 63]. For uint32_t/int32_t/float, channelSize can be 4, N * 8, N * 8 + 4; for half/bfloat16, channelSize can be 4, 8, N * 16, N * 16 + 4, N * 16 + 8; for int8_t/uint8_t, channelSize can be 4, 8, 16, 32 * N, N * 32 + 4, N * 32 + 8, N * 32 + 16; for int4b_t, ChannelSize can be 8, 16, 32, N * 64, N * 64 + 8, N * 64 + 16, N * 64 + 32. N is a positive integer.|

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- This API does not support the transpose scenario of 2D format transfer. To implement it, see [asc_copy_l12l0b_trans](../cube_datamove/cube_compute_load/asc_copy_l12l0b_trans_arch_2201.md).

## 3D Data Format Description

The input feature map and filter must be in the NC1HWC0 format, where C0 is the lowest dimension and is fixed at 16 (32 for the u8/s8 type), and C1 = C/C0.

To simplify the scenario, the following scenario assumes that the input feature map has 4 channels, that is, Ci = 4. The shape of the input feature maps in the L1 Buffer is (Hi, Wi, Ci). After load3dv1 processing, the data shape in the L0B Buffer is (Wo\*Ho, Hk\*Wk\*Ci). Wo and Ho are the output shapes after convolution, and Hk and Wk are the filter shapes.

Intuitively, the img2col process is one in which the filter sweeps across the feature map and expands the corresponding feature map data into each row of the output data. The filter first slides Wo steps in the W direction, then moves one step in the H direction and repeats the above process, finally outputting Wo * Ho rows of data. In the following figure, the red and yellow data represent the first row and the second row, respectively. The numbers indicate the association among the original input data, the filter, and the output data. It can be seen that load3dv1 first transfers the 4 numbers corresponding to 00 in the Ci dimension of the input data, and then transfers the 4 numbers corresponding to 01. Finally, the size of this row is Hk\*Wk\*Ci, that is, 3\*3\*4=36 numbers.

- The corresponding feature map format is shown in the following figure:

![ ](../../../figures/load3d_01.png)

- The corresponding filter format is shown in the following figure:

![ ](../../../figures/load3d_02.png)

Here, n is the number of filters. It can be seen that the dimension layout is (Hk, Wk, Ci, n). However, note that the format in the following figure also needs to be converted according to the B matrix format in Mmad.

In actual operation, due to storage space or computing capability limitations, we usually divide the entire convolution computation into blocks and transfer and compute only a small block of data at a time.

![ ](../../../figures/load3d_03.png)

For the feature map in the L0B Buffer, there are two schemes: horizontal blocking and vertical blocking, corresponding to repeatMode values 0 and 1 in the parameters, respectively.

Note: The fractal matrix size in the following figures is 4x4, but it should actually be 16x16 (16x32 for the u8/s8 type).

When repeatMode = 0, each repeat changes the position of the data point read in the filter window, and then jumps to the next C0 position.

![ ](../../../figures/load3d_04.png)

When repeatMode = 1, the position of the data read in the filter window remains unchanged, and each repeat advances by C0 elements in the feature map.

![ ](../../../figures/load3d_05.png)

## Examples

```cpp
__cbuf__ half src[256];
__cb__ half dst[256];
asc_copy_l12l0b_sync(dst, src, 1, 1, 1, 0);
```

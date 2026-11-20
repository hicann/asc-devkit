---
okftype: reference.api
description: "Moves image data from Global Memory to the dst start address of the L1 Buffer according to the horizontal and vertical start positions and load size, and can pad a specified number of pixels on the top, bottom, left, and right sides of the destination image."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/overview/cube_compute_flow.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0a_arch_2201.md
    kind: related
    condition: "Only on Atlas A3 and Atlas A2 products is there a subsequent L1-to-L0 3D data movement connection"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_arch_2201.md
    kind: related
    condition: "Only on Atlas A3 and Atlas A2 products is there a subsequent 3D data movement from L1 to L0"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "Storage unit space and alignment requirements, generic address overlap constraint"
version: ">=9.0.0"
version_changes: []
---
# asc_load_image_to_cbuf

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:27:17.712Z pushedAt=2026-11-18T08:15:34.320Z nl=2 -->

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

The header file path is `"c_api/cube_datamove/cube_datamove.h"`.

Moves image data from Global Memory to L1 Buffer.

For the position of image loading in the convolution computation flow, see [Cube Computation Flow](../../cube_compute/overview.md#conv2d-forward-propagation-flow).
<!-- npu="A3,910b" id8 -->
For the subsequent 3D move from L1 to L0, see [asc_copy_l12l0a (Atlas A3 products and Atlas A2 products)](../cube_compute_load/asc_copy_l12l0a_arch_2201.md) and [asc_copy_l12l0b (Atlas A3 products and Atlas A2 products)](../cube_compute_load/asc_copy_l12l0b_arch_2201.md).
<!-- end id8 -->

## Prototype

- Regular move

    ```cpp
    __aicore__ inline void asc_load_image_to_cbuf(__cbuf__ int8_t* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    __aicore__ inline void asc_load_image_to_cbuf(__cbuf__ half* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| hor_size | Input | Horizontal width of the image loaded from the source image, in pixels. Value range: [2, 4094]. |
| ver_size | Input | Vertical height of the image loaded from the source image, in pixels. Value range: [2, 4094]. |
| hor_start_pos | Input | Horizontal start address of the loaded image on the source image, in pixels. Value range: [0, 4095]. |
| ver_start_pos | Input | Vertical start address of the loaded image on the source image, in pixels. Value range: [0, 4095]. |
| src_hor_size | Input | Horizontal width of the source image, in pixels. Value range: [2, 4094]. |
| top_pad_size | Input | Number of pixels padded at the top of the destination image. Value range: [0, 32]. |
| bot_pad_size | Input | Number of pixels padded at the bottom of the destination image. Value range: [0, 32]. |
| left_pad_size | Input | Number of pixels padded on the left of the destination image. Value range: [0, 32]. |
| right_pad_size | Input | Number of pixels padded on the right of the destination image. Value range: [0, 32]. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the total length of data involved in the transfer.
__cbuf__ half dst[total_length];
// When loading the image, both the height and width are 1, the start address is (0,0), the horizontal width of the source image is 1, and no padding is performed.
asc_load_image_to_cbuf(dst, 1, 1, 0, 0, 1, 0, 0, 0, 0);
```

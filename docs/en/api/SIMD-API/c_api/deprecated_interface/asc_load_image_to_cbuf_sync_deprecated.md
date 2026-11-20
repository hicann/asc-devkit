---
okftype: reference.api
description: "Deprecated synchronous image load interface that moves image data from Global Memory into L1 Buffer and supports padding in the top, bottom, left, and right directions. It provides two destination type overloads, int8_t and half. The replacement is asc_load_image_to_cbuf combined with asc_sync."
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_load_image_to_cbuf.md
    kind: replaced_by
    condition: "The asynchronous interface that replaces this synchronous image load interface"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "For storage unit space and alignment requirements, and operand address overlap constraints, see the corresponding section on that page"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: related
    condition: "Works with the non-synchronous interface to complete synchronization"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "Both destination-type overloads carry the ASC_DEPRECATED(9.2.0, 2027/09/07, asc_load_image_to_cbuf and asc_sync) marker (the macro line before the corresponding @override_id in include/c_api/composite/cube_datamove_composite.h). The replacement is the non-synchronous interface asc_load_image_to_cbuf plus asc_sync. All prototypes on this page fall within the deprecation scope, which is page-level deprecation, and no usable upper limit is imposed."
---
# asc_load_image_to_cbuf_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:05:51.349Z pushedAt=2026-11-18T03:12:16.589Z nl=2 -->

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

**The `asc_load_image_to_cbuf_sync` API is deprecated. Use the [corresponding non-synchronous API](../cube_datamove/cube_load_aux_config/asc_load_image_to_cbuf.md) together with the synchronous API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Moves image data from Global Memory to L1 Buffer.

## Prototype

- Synchronous data movement

    ```cpp
    __aicore__ inline void asc_load_image_to_cbuf_sync(__cbuf__ int8_t* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    __aicore__ inline void asc_load_image_to_cbuf_sync(__cbuf__ half* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| hor_size | Input | Horizontal width of the image loaded from the source image, in pixels. Value range: [1, 4094]. |
| ver_size | Input | Vertical height of the image loaded from the source image, in pixels. Value range: [1, 4094]. |
| hor_start_pos | Input | Horizontal start address of the loaded image on the source image, in pixels. Value range: [0, 4095]. |
| ver_start_pos | Input | Vertical start address of the loaded image on the source image, in pixels. Value range: [0, 4095]. |
| src_hor_size | Input | Horizontal width of the source image, in pixels. Value range: [1, 4094]. |
| top_pad_size | Input | Pixel count padded at the top of the destination image. Value range: [0, 32]. |
| bot_pad_size | Input | Pixel count padded at the bottom of the destination image. Value range: [0, 32]. |
| left_pad_size | Input | Pixel count padded on the left of the destination image. Value range: [0, 32]. |
| right_pad_size | Input | Pixel count padded on the right of the destination image. Value range: [0, 32]. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Examples

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the total length of data involved in the movement.
__cbuf__ half dst[total_length];
// When loading the image, the height and width are both 1, the start address is (0,0), the source image horizontal width is 1, and no padding is performed.
asc_load_image_to_cbuf_sync(dst, 1, 1, 0, 0, 1, 0, 0, 0, 0);
```

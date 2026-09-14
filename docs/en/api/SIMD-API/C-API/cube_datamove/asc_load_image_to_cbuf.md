# asc_load_image_to_cbuf

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:52:48.932Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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

Copies image data from **Global Memory** to **L1 Buffer**.

## Prototype

- Regular copy

    ```cpp
    __aicore__ inline void asc_load_image_to_cbuf(__cbuf__ int8_t* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    __aicore__ inline void asc_load_image_to_cbuf(__cbuf__ half* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    ```

- Synchronous copy

    ```cpp
    __aicore__ inline void asc_load_image_to_cbuf_sync(__cbuf__ int8_t* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    __aicore__ inline void asc_load_image_to_cbuf_sync(__cbuf__ half* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos, uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| **dst** | Output | Start address of the destination operand (vector). |
| **hor_size** | Input | Horizontal width of the image loaded from the source image, in pixels. Value range: [1, 4094]. |
| **ver_size** | Input | Vertical height of the image loaded from the source image, in pixels. Value range: [1, 4094]. |
| **hor_start_pos** | Input | Horizontal start position of the loaded image on the source image, in pixels. Value range: [0, 4095]. |
| **ver_start_pos** | Input | Vertical start position of the loaded image on the source image, in pixels. Value range: [0, 4095]. |
| **src_hor_size** | Input | Horizontal width of the source image, in pixels. Value range: [1, 4094]. |
| **top_pad_size** | Input | Number of pixels padded at the top of the destination image. Value range: [0, 32]. |
| **bot_pad_size** | Input | Number of pixels padded at the bottom of the destination image. Value range: [0, 32]. |
| **left_pad_size** | Input | Number of pixels padded on the left of the destination image. Value range: [0, 32]. |
| **right_pad_size** | Input | Number of pixels padded on the right of the destination image. Value range: [0, 32]. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- The start address of **dst** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
constexpr uint64_t total_length = 128;    // total_length indicates the total length of the data involved in the copy.
__cbuf__ half dst[total_length];
// When loading the image, the height and width are both 1, the start address is (0,0), the source image horizontal width is 1, and no padding is applied.
asc_load_image_to_cbuf(dst, 1, 1, 0, 0, 1, 0, 0, 0, 0);
```

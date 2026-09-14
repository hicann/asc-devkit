# asc_copy_l12l0b_mx

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:43:31.592Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

Copies the Mx scale matrix from **L1 Buffer** to **L0B Buffer**.

## Prototype

- Regular copy

    ```cpp
    __aicore__ inline void asc_copy_l12l0b_mx(uint64_t dst, __cbuf__ fp8_e8m0_t* src, uint16_t x_start_pos, uint16_t y_start_pos, uint8_t x_step, uint8_t y_step, uint16_t src_stride, uint16_t dst_stride)
    ```

- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_l12l0b_mx_sync(uint64_t dst, __cbuf__ fp8_e8m0_t* src, uint16_t x_start_pos, uint16_t y_start_pos, uint8_t x_step, uint8_t y_step, uint16_t src_stride, uint16_t dst_stride)
    ```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| **dst**       | Output | Address of the destination operand. |
| **src** | Input | Start address of the source operand (Mx scale matrix), with the data type **fp8_e8m0_t**. |
| **x_start_pos** | Input | Start position of the source operand along the X-axis, that is, the M dimension direction, in units of one fractal (one unit represents a 32B fractal). |
| **y_start_pos** | Input | Start position of the source operand along the Y-axis, that is, the K dimension direction, in units of 32B. |
| **x_step** | Input | Copy length of the source operand along the X-axis, that is, the M dimension direction, in units of one fractal (one unit represents a 32B fractal). Value range: [0, 255]. |
| **y_step** | Input | Copy length of the source operand along the Y-axis, that is, the K dimension direction, in units of 32B. Value range: [0, 255].|
| **src_stride** | Input | Interval between the start address of the previous fractal and that of the next fractal along the X-axis of the source operand, in units of 32B. |
| **dst_stride** | Input | Interval between the start address of the previous fractal and that of the next fractal along the X-axis of the destination operand, in units of 32B. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- The fractal of the scale matrix is fixed at (16, 2, 16), corresponding to a fractal of (16, 32, 16) in the L0B Buffer, which occupies 1/16 of the L0B Buffer address. Addresses must be stored according to this address constraint.
- The data type of **dst** (the address of the L0B Buffer) supports fp4x2_e2m1_t, fp4x2_e1m2_t, fp8_e5m2_t, and fp8_e4m3fn_t, and the data type of **src** supports fp8_e8m0_t.
- The start addresses of **dst** and **src** must be 32-byte aligned.

## Example

```cpp
__cbuf__ fp8_e8m0_t src[1024];
__cb__ fp8_e5m2_t dst[1024];
uint16_t x_start_pos = 0;
uint16_t y_start_pos = 0;
uint8_t x_step = 0;
uint8_t y_step = 0;
uint16_t src_stride = 8;
uint16_t dst_stride = 8;
uint64_t mx_dst_addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 16;
asc_copy_l12l0b_mx(mx_dst_addr, src, x_start_pos, y_start_pos, x_step, y_step, src_stride, dst_stride);
```

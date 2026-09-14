# asc_create_iter_reg

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:20:25.523Z pushedAt=2026-09-05T12:14:28.800Z -->

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

The address register is initialized through this API and then used within the loop to store the address offset. This overloaded API supports 1 to 4 **offset** parameters simultaneously, and supports initializing registers of b8/b16/b32 bit width, adapting to 1- to 4-dimensional tensors. The underlying layer automatically computes the address offset in multi-dimensional loops, eliminating scalar address computation overhead.

## Prototype

  ```cpp
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset0, uint32_t offset1)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset0, uint32_t offset1)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset0, uint32_t offset1)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset0, uint32_t offset1, uint32_t offset2)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset0, uint32_t offset1, uint32_t offset2)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset0, uint32_t offset1, uint32_t offset2)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b32(uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b16(uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3)
  __simd_callee__ inline iter_reg asc_create_iter_reg_b8(uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3)
  ```

## Parameters

| Parameter       | Input/Output | Description                |
| --------- | ----- | ----------------- |
| offset       | Input | Address offset. |
| offset0      | Input | Address offset. |
| offset1     | Input | Address offset. |
| offset2     | Input | Address offset. |
| offset3     | Input | Address offset. |

## Return Value

Address register.

## Pipeline Type

PIPE_V

## Constraints

- Do not use if/else inside the loop.
- **iter_reg** must be assigned and used within the innermost loop body.
- For operands with different strides, use multiple **iter_reg** instances to avoid address overwriting.

## Example

For details, see [address register call example](../reg_data_definition.md#example-3).

# asc_uint162uint8

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:25:22.449Z pushedAt=2026-09-05T12:14:28.949Z -->

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

Converts the uint16_t type to the uint8_t type without a rounding mode.

Overflow modes:
- Non-saturation mode: The result is truncated to the data width of the destination format, and the least significant bits (LSBs) are retained.
- Saturation mode: The result is clamped to the representable range of the destination format.

Because the bit-width ratio between the source operand and the destination operand is 2:1, the data of one VL size must be divided into two parts when written, and index 0 or index 1 is selected depending on the API.

## Prototype

```cpp
// Non-saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_uint162uint8(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
// Saturation mode, data written to even-indexed positions.
__simd_callee__ inline void asc_uint162uint8_sat(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
// Non-saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_uint162uint8_v2(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
// Saturation mode, data written to odd-indexed positions.
__simd_callee__ inline void asc_uint162uint8_sat_v2(vector_uint8_t& dst, vector_uint16_t src, vector_bool mask)
```

## Parameters

| Parameter    | Input/Output | Description                |
| :------ | :----- | :----------------- |
| dst    | Output    | Destination operand (vector data register).            |
| src    | Input    | Source operand (vector data register).             |
| mask | Input | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- Non-saturation mode: When the input data exceeds the target data range, it is truncated to the target data format. For example, if the input uint16_t value is 256, the output uint8_t value is 0.
- Saturation mode: When the input data exceeds the target data range, the corresponding extreme value of the output type is returned.
- To enable saturation mode or non-saturation mode, configure the ctrl register. For details about the ctrl register, see [asc_set_ctrl.md](../../sys_var/asc_set_ctrl.md).

## Example

```cpp
vector_uint16_t src;
vector_uint8_t dst;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the address of the externally input UB memory space.
asc_uint162uint8(dst, src, mask);
```

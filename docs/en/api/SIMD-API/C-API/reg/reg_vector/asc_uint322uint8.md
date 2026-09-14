# asc_uint322uint8

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:28:09.482Z pushedAt=2026-09-05T12:14:28.953Z -->

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

Converts data of the uint32_t type to the uint8_t type.

The conversion is classified into non-saturation mode and saturation mode based on the size of the input data.
- Non-saturation mode: The result is truncated to the data width of the target format, and the least significant bits (LSBs) are retained.
- Saturation mode: The result is clamped to the representable range of the target format.

Because the bit-width ratio between the source operand and the destination operand is 4:1, when writing data, one VL of data must be divided into four parts, and index 0, index 1, index 2, or index 3 is selected according to the API.

## Prototype

```cpp
// Non-saturation mode. Data is written to index 0.
__simd_callee__ inline void asc_uint322uint8(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Saturation mode. Data is written to index 0.
__simd_callee__ inline void asc_uint322uint8_sat(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Non-saturation mode. Data is written to index 1.
__simd_callee__ inline void asc_uint322uint8_v2(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Saturation mode. Data is written to index 1.
__simd_callee__ inline void asc_uint322uint8_sat_v2(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Non-saturation mode. Data is written to index 2.
__simd_callee__ inline void asc_uint322uint8_v3(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Saturation mode. Data is written to index 2.
__simd_callee__ inline void asc_uint322uint8_sat_v3(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Non-saturation mode. Data is written to index 3.
__simd_callee__ inline void asc_uint322uint8_v4(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
// Saturation mode. Data is written to index 3.
__simd_callee__ inline void asc_uint322uint8_sat_v4(vector_uint8_t& dst, vector_uint32_t src, vector_bool mask)
```

## Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
| dst        | Output    | Destination operand (vector data register). |
| src        | Input    | Source operand (vector data register). |
| mask       | Input    | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- When saturation mode and non-saturation mode are enabled, the ctrl register must be configured. For details about the ctrl register, see [asc_set_ctrl.md](../../sys_var/asc_set_ctrl.md).

## Example

```cpp
vector_uint32_t src;
vector_uint8_t dst;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the address of the UB memory space input externally.
asc_uint322uint8(dst, src, mask);
```

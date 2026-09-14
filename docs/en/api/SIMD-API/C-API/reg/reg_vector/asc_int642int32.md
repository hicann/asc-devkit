# asc_int642int32

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T10:29:37.694Z pushedAt=2026-09-05T12:14:28.884Z -->

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

Converts a source operand of the vector_int64_t type into the vector_int32_t type, writes the result to the upper half or lower half of the destination operand, and supports different saturation modes:

- **asc_int642int32**: Non-saturation mode, writes to the upper half of the destination operand.

- **asc_int642int32_sat**: Saturation mode, writes to the upper half of the destination operand.

- **asc_int642int32_v2**: Non-saturation mode, writes to the lower half of the destination operand.

- **asc_int642int32_sat_v2**: Saturation mode, writes to the lower half of the destination operand.

## Prototype

```cpp
__simd_callee__ inline void asc_int642int32(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
__simd_callee__ inline void asc_int642int32_sat(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
__simd_callee__ inline void asc_int642int32_v2(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
__simd_callee__ inline void asc_int642int32_sat_v2(vector_int32_t& dst, vector_int64_t src, vector_bool mask)
```

## Parameters

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| dst       | Output    | Destination operand (vector data register).            |
| src | Input    | Source operand (vector data register).            |
| mask     | Input    | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output.        |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- When saturation mode and non-saturation mode are enabled, the **ctrl** register must be configured. For details about the **ctrl** register, see [asc_set_ctrl.md](../../sys_var/asc_set_ctrl.md).

## Example

```cpp
vector_int32_t dst;
vector_int64_t src;
vector_bool mask;
mask = asc_create_mask_b32(PAT_ALL); // Currently, the mask supports only b8, b16, and b32 modes. A full mask of a lower bit width can be used as a full mask for a higher-bit-width data type.
asc_int642int32(dst, src, mask);    // Non-saturation mode. Convert src to the vector_int32_t type and write the result to the upper half of dst.
asc_int642int32_sat(dst, src, mask);    // Saturation mode. Convert src to the vector_int32_t type and write it to the upper half of dst.
asc_int642int32_v2(dst, src, mask);    // Non-saturation mode. Convert src to the vector_int32_t type and write it to the lower half of dst.
asc_int642int32_sat_v2(dst, src, mask);    // Saturation mode. Convert src to the vector_int32_t type and write it to the lower half of dst.
```
# asc_unpack

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:33:26.963Z pushedAt=2026-09-05T12:14:28.959Z -->

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

Vector unpacking operation. For unsigned integers, the elements in the lower half or upper half of the source operand are written to the destination operand with their bit width expanded by zero-filling the high bits. For signed integers, the elements in the lower half or upper half of the source operand are written to the destination operand with their bit width expanded by sign extension. For boolean vectors, the elements in the lower half or upper half of the source operand are written to the destination operand while preserving their truth values and original order.

## Prototype

```cpp
__simd_callee__ inline void asc_unpack_upper(vector_uint16_t& dst, vector_uint8_t src)
__simd_callee__ inline void asc_unpack_lower(vector_uint16_t& dst, vector_uint8_t src)
__simd_callee__ inline void asc_unpack_upper(vector_int16_t& dst, vector_int8_t src)
__simd_callee__ inline void asc_unpack_lower(vector_int16_t& dst, vector_int8_t src)
__simd_callee__ inline void asc_unpack_upper(vector_uint32_t& dst, vector_uint16_t src)
__simd_callee__ inline void asc_unpack_lower(vector_uint32_t& dst, vector_uint16_t src)
__simd_callee__ inline void asc_unpack_upper(vector_int32_t& dst, vector_int16_t src)
__simd_callee__ inline void asc_unpack_lower(vector_int32_t& dst, vector_int16_t src)
__simd_callee__ inline void asc_unpack_upper(vector_bool& dst, vector_bool src)
__simd_callee__ inline void asc_unpack_lower(vector_bool& dst, vector_bool src)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Destination operand (vector data register/mask register). |
| src | Input | Source operand (vector data register/mask register). |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_uint16_t dst;
vector_uint8_t src;
asc_loadalign(src, src_addr); // src_addr is the externally input UB memory space address.
asc_unpack_upper(dst, src);
```
# asc_pack

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:00:54.113Z pushedAt=2026-09-05T12:14:28.920Z -->

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

Selects the low 8 bits (b16), low 16 bits (b32), or low 32 bits (b64) of each element in the source operand and writes them into the lower half or upper half of the destination operand.

- **asc_pack**: Writes the source operand into the lower half of the destination operand.

- **asc_pack_v2**: Writes the source operand into the upper half of the destination operand.

## Prototype

```cpp
__simd_callee__ inline void asc_pack(vector_uint8_t& dst, vector_uint16_t src)
__simd_callee__ inline void asc_pack(vector_uint8_t& dst, vector_int16_t src)
__simd_callee__ inline void asc_pack(vector_uint16_t& dst, vector_uint32_t src)
__simd_callee__ inline void asc_pack(vector_uint16_t& dst, vector_int32_t src)
__simd_callee__ inline void asc_pack(vector_bool& dst, vector_bool src)
__simd_callee__ inline void asc_pack_v2(vector_uint8_t& dst, vector_uint16_t src)
__simd_callee__ inline void asc_pack_v2(vector_uint8_t& dst, vector_int16_t src)
__simd_callee__ inline void asc_pack_v2(vector_uint16_t& dst, vector_uint32_t src)
__simd_callee__ inline void asc_pack_v2(vector_uint16_t& dst, vector_int32_t src)
__simd_callee__ inline void asc_pack_v2(vector_bool& dst, vector_bool src)
```

## Parameters

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| dst       | Output    | Destination operand (vector data register/mask register).            |
| src | Input    | Source operand (vector data register/mask register).            |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_uint8_t dst;
vector_uint16_t src;
asc_pack(dst, src);    // Write the low 8 bits of src into the lower half of dst.
asc_pack_v2(dst, src);    // Write the low 8 bits of src into the upper half of dst.
```
# asc_int322int64

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T10:20:07.481Z pushedAt=2026-09-05T12:14:28.866Z -->

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

Divides the source operand of the `vector_int32_t` type into two parts in units of 256B, reads the elements of one part, converts them to the `vector_int64_t` type, and writes them to the destination operand.

- asc_int322int64: Writes the upper half of the source operand to the destination operand.

- asc_int322int64_v2: Writes the lower half of the source operand to the destination operand.

## Prototype

```cpp
__simd_callee__ inline void asc_int322int64(vector_int64_t& dst, vector_int32_t src, vector_bool mask)
__simd_callee__ inline void asc_int322int64_v2(vector_int64_t& dst, vector_int32_t src, vector_bool mask)
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

None

## Example

```cpp
vector_int64_t dst;
vector_int32_t src;
vector_bool mask;
mask = asc_create_mask_b32(PAT_ALL);
asc_int322int64(dst, src, mask);    // Convert the upper half of src to the vector_int64_t type and write it to dst.
asc_int322int64_v2(dst, src, mask);    // Convert the lower half of src to the vector_int64_t type and write it to dst.
```
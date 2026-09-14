# asc_e1m2x22bfloat16

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:28:18.141Z pushedAt=2026-09-05T12:14:28.810Z -->

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

Converts the **fp4x2_e1m2_t** type to the **bfloat16_t** type.

Since the bit-width ratio of the source operand type to the destination operand type is 1:4, one VL of data must be divided into four parts when being read. Depending on the API, the part at index 0, 1, 2, or 3 is selected.

## Prototype

```cpp
// Read data at index 0.
__simd_callee__ inline void asc_e1m2x22bfloat16(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
// Read data at index 1.
__simd_callee__ inline void asc_e1m2x22bfloat16_v2(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
// Read data at index 2.
__simd_callee__ inline void asc_e1m2x22bfloat16_v3(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
// Read data at index 3.
__simd_callee__ inline void asc_e1m2x22bfloat16_v4(vector_bfloat16_t& dst, vector_fp4x2_e1m2_t src, vector_bool mask)
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

None

## Example

```cpp
vector_fp4x2_e1m2_t src;
vector_bfloat16_t dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the address of the UB memory space input from outside.
asc_e1m2x22bfloat16(dst, src, mask);
```

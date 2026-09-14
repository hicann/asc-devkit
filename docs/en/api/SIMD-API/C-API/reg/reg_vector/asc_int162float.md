# asc_int162float

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T10:11:17.742Z pushedAt=2026-09-05T12:14:28.855Z -->

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

Converts the **int16_t** type to the **float** type.

Because the bit-width ratio between the source operand and the destination operand is 1:2, when reading data, one **VL**-sized data block must be divided into two parts, and index 0 or index 1 is selected according to the API.

## Prototype

```cpp
// Read data at even indexes.
__simd_callee__ inline void asc_int162float(vector_float& dst, vector_int16_t src, vector_bool mask)
// Read data at odd indexes.
__simd_callee__ inline void asc_int162float_v2(vector_float& dst, vector_int16_t src, vector_bool mask)
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
vector_int16_t src;
vector_float dst;
vector_bool mask = asc_create_mask_b16(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input UB memory space address.
asc_int162float(dst, src, mask);
```

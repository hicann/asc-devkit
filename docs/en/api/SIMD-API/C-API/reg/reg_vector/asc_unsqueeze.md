# asc_unsqueeze

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:35:38.560Z pushedAt=2026-09-05T12:14:28.960Z -->

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

Performs decompression based on **mask** and outputs the generated data to **dst**. Decompression method: the 0th element of **dst** is set to 0, and the i-th element of **dst** equals the number of 1s in **mask** from the 0th to the (i-1)-th element. The most significant bit of **mask** is ignored and does not participate in the statistics. The algorithm logic is expressed as follows:
  ```cpp
  dst[0] = 0;
  // dstLen is the number of elements contained in dst.
  for (int i = 1; i < dstLen; i++) {
    dst[i] = mask[i - 1] ? (dst[i - 1] + 1) : dst[i - 1];
  }
  ```

## Prototype

  ```cpp
  __simd_callee__ inline void asc_unsqueeze(vector_int8_t& dst, vector_bool mask)
  __simd_callee__ inline void asc_unsqueeze(vector_uint8_t& dst, vector_bool mask)
  __simd_callee__ inline void asc_unsqueeze(vector_int16_t& dst, vector_bool mask)
  __simd_callee__ inline void asc_unsqueeze(vector_uint16_t& dst, vector_bool mask)
  __simd_callee__ inline void asc_unsqueeze(vector_int32_t& dst, vector_bool mask)
  __simd_callee__ inline void asc_unsqueeze(vector_uint32_t& dst, vector_bool mask)
  ```

## Parameters

| Parameter       | Input/Output | Description                |
| --------- | ----- | ----------------- |
| dst       | Output    | Destination operand (vector data register). |
| mask      | Input    | Source operand mask (mask register), used to provide decompression information for dstReg. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_int8_t dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_unsqueeze(dst, mask);
```

# asc_arange

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:07:59.032Z pushedAt=2026-09-05T12:14:28.786Z -->

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

Generates ascending/descending indices starting from the passed-in **value** and stores the generated indices in **dst**. The algorithm logic is as follows:
  ```cpp
  // Ascending.
  {value, value + 1, value + 2, ... value + VL_T - 2, value + VL_T - 1}
  // Descending.
  {value + VL_T - 1, value + VL_T - 2, value + VL_T - 3, ... value + 1, value}
  ```

Taking the int16_t data type with a start value of 10 as an example: the ascending indices are {10, 11, 12, 13, ... 135, 136, 137}, and the descending indices are {137, 136, 135, 134, ... 12, 11, 10}.

## Prototype

  ```cpp
  // Ascending mode.
  __simd_callee__ inline void asc_arange(vector_int8_t& dst, int8_t value)
  __simd_callee__ inline void asc_arange(vector_int16_t& dst, int16_t value)
  __simd_callee__ inline void asc_arange(vector_half& dst, half value)
  __simd_callee__ inline void asc_arange(vector_int32_t& dst, int32_t value)
  __simd_callee__ inline void asc_arange(vector_float& dst, float value)
  // Descending mode.
  __simd_callee__ inline void asc_arange_descend(vector_int8_t& dst, int8_t value)
  __simd_callee__ inline void asc_arange_descend(vector_int16_t& dst, int16_t value)
  __simd_callee__ inline void asc_arange_descend(vector_half& dst, half value)
  __simd_callee__ inline void asc_arange_descend(vector_int32_t& dst, int32_t value)
  __simd_callee__ inline void asc_arange_descend(vector_float& dst, float value)
  ```

## Parameters

| Parameter | Input/Output | Description |
| --------- | ----- | ----------------- |
| dst | Output | Destination operand (vector data register). |
| value | Input | Source operand (scalar). |

For details about the vector data register, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_int8_t dst;
int8_t value = 0;
asc_arange(dst, value);
```

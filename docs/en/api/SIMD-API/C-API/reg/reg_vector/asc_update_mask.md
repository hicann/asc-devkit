# asc_update_mask

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T11:35:44.535Z pushedAt=2026-09-05T12:14:28.962Z -->

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

Generates the corresponding value in the mask register based on the value size. The valid element range of the mask register is from 0 to VL_T (the number of elements of the corresponding data type whose bit width is Vector Length). After this function is executed, value is decremented by VL_T. The algorithm logic is as follows:
  ```cpp
  value = (value < VL_T) ? 0 : (value - VL_T);
  ```

## Prototype

  ```cpp
  __simd_callee__ inline vector_bool asc_update_mask_b8(uint32_t& value)
  __simd_callee__ inline vector_bool asc_update_mask_b16(uint32_t& value)
  __simd_callee__ inline vector_bool asc_update_mask_b32(uint32_t& value)
  ```

## Parameters

| Parameter       | Input/Output | Description                |
| --------- | ----- | ----------------- |
| value       | Input/Output | Specific number of elements to be operated on in vector computation. |

## Return Value

Mask register. If value is greater than or equal to VL_T, all element positions are set to 1; if value is less than VL_T, the corresponding element positions from bit 0 to bit value-1 are set to 1.

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
uint32_t value = 127;
vector_bool mask = asc_create_mask_b32(PAT_ALL);
// A total of 127 elements need to be computed, which requires 2 VLs.
for (int32_t i = 0; i < 2; i++) {
  mask = asc_update_mask_b32(value);
  // Use the mask to perform computation of one VL.
}
```

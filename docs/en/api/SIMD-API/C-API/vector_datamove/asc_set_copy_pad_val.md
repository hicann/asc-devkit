# asc_set_copy_pad_val

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-08T04:43:43.700Z pushedAt=2026-09-08T13:17:42.525Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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

Used together with **asc_copy_gm2ub_align** or **asc_copy_ub2gm_align** to set the data values to be padded on the left and right sides of a contiguous data block being copied.

<!-- npu="950" id8 -->

For Ascend 950PR/Ascend 950DT:
- This API is effective only for the **asc_copy_gm2ub_align** API.
- Data of the fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, fp4x2_e2m1_t, fp4x2_e1m2_t, and hifloat8_t types must be converted to the int8_t type before this API is called.
<!-- end id8 -->

## Prototype

```cpp
__aicore__ inline void asc_set_copy_pad_val(int8_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(uint8_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(int16_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(uint16_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(half pad_value)
__aicore__ inline void asc_set_copy_pad_val(bfloat16_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(int32_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(uint32_t pad_value)
__aicore__ inline void asc_set_copy_pad_val(float pad_value)
```

## Parameters

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| pad_value     | Input     | Data value to be padded.|

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
// The source operand is not aligned, and data needs to be padded.
asc_set_copy_pad_val(0);
asc_copy_gm2ub_align(dst, src, 2, 48 * sizeof(int8_t), 0, 0, true, 0, 0, 0);
```

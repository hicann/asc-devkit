# asc_set_ndim_pad_value

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-08T04:54:06.482Z pushedAt=2026-09-08T13:17:42.535Z -->

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

When the **padding_mode** parameter of the [asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md) API is set to true, this value is used as the fixed padding value. Data of the **fp8_e8m0_t**, **fp8_e5m2_t**, and **fp8_e4m3fn_t** types must be converted to the **int8_t** type before calling this API.

## Prototype

```c++
__aicore__ inline void asc_set_ndim_pad_value(int8_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(uint8_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(int16_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(uint16_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(half pad_value)
__aicore__ inline void asc_set_ndim_pad_value(bfloat16_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(int32_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(uint32_t pad_value)
__aicore__ inline void asc_set_ndim_pad_value(float pad_value)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| pad_value | Input | Fixed padding value used to set the padding of the **asc_ndim_copy_gm2ub** API. The data type must be consistent with the data types of **dst** and **src** in the **asc_ndim_copy_gm2ub** API. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

Must be used together with [asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md).

## Example

```c++
half pad_value = 2.0;
asc_set_ndim_pad_value(pad_value); 
```
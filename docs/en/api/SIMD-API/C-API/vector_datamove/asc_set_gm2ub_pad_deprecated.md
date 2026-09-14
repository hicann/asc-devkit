# asc_set_gm2ub_pad (Deprecated)

<!-- md-trans-meta sourceCommit=146cb514e729127bc432adbcca9b425ed937b76d translatedAt=2026-09-08T04:49:48.388Z pushedAt=2026-09-08T13:17:42.531Z -->

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

**This API is deprecated. To set the padding values when copying source data from GM to UB in a non-aligned manner, use [asc_set_copy_pad_val](./asc_set_copy_pad_val.md).**

When copying data from Global Memory (GM) to the Unified Buffer (UB) in a non-aligned manner, call this API to set the padding values for the left and right sides of the consecutively copied data blocks.

## Prototype

```cpp 
__aicore__ inline void asc_set_gm2ub_pad(uint32_t pad_val)
``` 

## Parameters

|Parameter|Input/Output|Description| 
|------------|------------|-----------| 
| **pad_val**    | Input       | Padding values for the left and right sides.|

## Return Value

None 

## Pipeline Type

PIPE_S

## Constraints

None 

## Example

```cpp 
asc_set_gm2ub_loop_size(2, 2); 
asc_set_gm2ub_loop1_stride(96, 128); 
asc_set_gm2ub_loop2_stride(192, 288); 
// The source operand is non-aligned and requires padding.
asc_set_gm2ub_pad(0); 
asc_copy_gm2ub_align(dst, src, 2, 46 * sizeof(int8_t), 1, 1, true, 0, 48, 48); 
asc_set_gm2ub_loop_size(1, 1); 
```

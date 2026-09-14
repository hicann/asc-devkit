# asc_set_ndim_loop_stride

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-08T04:51:22.334Z pushedAt=2026-09-08T13:17:42.533Z -->

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

Sets the interval between elements of the source operand and the destination operand in each dimension for the [asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md) API, for up to 5 dimensions.

## Prototype

```cpp
__aicore__ inline void asc_set_ndim_loop0_stride(uint64_t dst_stride, uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop1_stride(uint64_t dst_stride, uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop2_stride(uint64_t dst_stride, uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop3_stride(uint64_t dst_stride, uint64_t src_stride)
__aicore__ inline void asc_set_ndim_loop4_stride(uint64_t dst_stride, uint64_t src_stride)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst_stride | Input | Interval between elements of the destination operand, in number of elements. Default value: 0. Value range: [0, 2^20 - 1]. |
| src_stride | Input | Interval between elements of the source operand, in number of elements. Default value: 0. Value range: [0, 2^40 - 1]. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

Must be used together with [asc_ndim_copy_gm2ub](./asc_ndim_copy_gm2ub.md).

## Example

```cpp
uint64_t dst_stride = 8;
uint64_t src_stride = 8;
asc_set_ndim_loop0_stride(dst_stride, src_stride);
```
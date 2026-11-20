---
okftype: reference.api
description: "Deprecated multi-dimensional data movement legacy overload that moves GM data into the Unified Buffer with up to 5-dimensional configuration, supporting per-dimension Stride and constant and nearest-value Padding. The last parameter is a uint8 cache_mode; the replacement uses the l2_cache_mode enum."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [high_dimensional, padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_load_l2_cache_mode.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md
    kind: replaced_by
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ndim_loop_stride.md
    kind: requires
    condition: "Per-dimension Stride must be configured through this API first"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ndim_pad_count.md
    kind: requires
    condition: "The number of left and right padding for dimensions 1 to 4 is configured through this API"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ndim_pad_value.md
    kind: requires
    condition: "When padding_mode is true, the constant fill value must be configured first"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The old overload with the last parameter uint8_t cache_mode is deprecated; use the asc_load_l2_cache_mode l2_cache_mode parameter instead. The 14 declarations starting from include/c_api/vector_datamove/vector_datamove.h:2181 all carry ASC_DEPRECATED(9.2.0), and the CANN_9_2_0 deprecation table lists the old overload that expresses the L2 Cache policy with an integer parameter."
---
# asc_ndim_copy_gm2ub (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:28:45.524Z pushedAt=2026-11-18T03:12:16.626Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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

**The following function prototype is deprecated. Use the `asc_load_l2_cache_mode` enum value to configure the L2 cache management policy. For the new API form, see [asc_ndim_copy_gm2ub](../vector_datamove/asc_ndim_copy_gm2ub.md).**

Multi-dimensional data movement API that moves data from Global Memory (GM) to Unified Buffer (UB). Compared with the basic data movement API, it allows more flexible configuration of the dimension information to be moved in.
This API can process up to 5 dimensions. Each dimension can be configured with a Stride independently, and on-the-fly Padding is supported. There are many related configuration parameters, which need to be passed in through different APIs.

**Table 1** All parameters involved in Stride configuration and the APIs through which they are passed in, where i indicates the i-th dimension and can take a value in [0,4]
| Parameter | API for passing in | Description |
| :--- | :------- | :--- |
| loopi_size | this API | Indicates the number of elements processed within the dimension (does not contain Padding elements).<br> The unit is the number of elements, and the value range is [1, 2^20-1]. |
| loopi_src_stride | [asc_set_ndim_loopi_stride](../vector_datamove/asc_set_ndim_loop_stride.md) | Indicates the interval between the source operand element and the next element within the dimension.<br> The unit is the number of elements, and the value range is [0, 2^40-1]. |
| loopi_dst_stride | [asc_set_ndim_loopi_stride](../vector_datamove/asc_set_ndim_loop_stride.md) | Indicates the interval between the destination operand element and the next element within the dimension.<br> The unit is the number of elements, and the value range is [0, 2^20-1]. |

**Table 2** All parameters involved in Padding configuration and the APIs through which they are passed in, where i indicates the i-th dimension and can take a value in [0,4]
| Parameter | API for passing in | Description |
| :--- | :------- | :--- |
| padding_mode | this API | Indicates the Padding value filling mode. The type is bool.<br>&bull; true: uses the constant filling mode, that is, all Padding values are the fixed value pad_value.<br>&bull; false: uses the nearest value filling mode, that is, the left and right Padding values select the leftmost or rightmost value of the current dimension for filling. |
| pad_value | [asc_set_ndim_pad_value](../vector_datamove/asc_set_ndim_pad_value.md) | When padding_mode is true, this value is used as the fixed Padding filling value. When padding_mode is false, this value does not take effect. |
| loopi_lp_count | <br>&bull; Dimension 0: this API <br>&bull; Dimensions 1 to 4: [asc_set_ndim_pad_count](../vector_datamove/asc_set_ndim_pad_count.md) | Indicates the number of elements to be padded on the left within the dimension.<br> The unit is the number of elements, and the value range is [0, 255]. |
| loopi_rp_count | <br>&bull; Dimension 0: this API <br>&bull; Dimensions 1 to 4: [asc_set_ndim_pad_count](../vector_datamove/asc_set_ndim_pad_count.md) | Indicates the number of elements to be padded on the right within the dimension.<br> The unit is the number of elements, and the value range is [0, 255]. |

The following uses a 2-dimensional example to introduce several typical usage scenarios.

- 2D Padding scenario, using the fixed value filling mode
![ ](../../../figures/asc_ndim_copy_gm2ub_2d_padding_constant.png)

- 2D Padding scenario, using nearest value filling mode
![ ](../../../figures/asc_ndim_copy_gm2ub_2d_padding_nearest.png)

- 2D Transpose scenario
![ ](../../../figures/asc_ndim_copy_gm2ub_2d_transpose.png)

- 2D Slice scenario
![ ](../../../figures/asc_ndim_copy_gm2ub_2d_slice.png)

- 2D Broadcast scenario
![ ](../../../figures/asc_ndim_copy_gm2ub_2d_broadcast.png)

## Prototype

```c
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ fp4x2_e2m1_t* dst, __gm__ fp4x2_e2m1_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ fp4x2_e1m2_t* dst, __gm__ fp4x2_e1m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ half* dst, __gm__ half* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ float* dst, __gm__ float* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size, uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode, uint8_t cache_mode)
```

## Parameters

**Table 3** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Destination UB address. |
| src | Input | Source GM address. |
| loop0_size | Input | Indicates the number of elements processed in dimension 0 (does not contain Padding elements).<br> The unit is the number of elements, and the value range is [1, 2^20-1]. |
| loop1_size | Input | Indicates the number of elements processed in dimension 1 (does not contain Padding elements).<br> The unit is the number of elements, and the value range is [1, 2^20-1]. |
| loop2_size | Input | Indicates the number of elements processed in dimension 2 (does not contain Padding elements).<br> The unit is the number of elements, and the value range is [1, 2^20-1]. |
| loop3_size | Input | Indicates the number of elements processed in dimension 3 (does not contain Padding elements).<br> The unit is the number of elements, and the value range is [1, 2^20-1]. |
| loop4_size | Input | Indicates the number of elements processed in dimension 4 (does not contain Padding elements).<br> The unit is the number of elements, and the value range is [1, 2^20-1]. |
| loop0_lp_count | Input | Indicates the number of elements to be padded on the left side of dimension 0.<br> The unit is the number of elements, and the value range is [0, 255]. |
| loop0_rp_count | Input | Indicates the number of elements to be padded on the right side of dimension 0.<br> The unit is the number of elements, and the value range is [0, 255]. |
| padding_mode | Input | Indicates the Padding value filling mode.<br>&bull; true: Uses the constant filling mode, that is, all Padding values are the fixed value pad_value.<br>&bull; false: Uses the nearest value filling mode, that is, the left and right Padding values select the leftmost or rightmost value of the current dimension for filling. |
| l2_cache_mode | Input | Enumeration value of the [asc_load_l2_cache_mode](../defs/enum/asc_load_l2_cache_mode.md) type, which configures the management policy of data in the L2 Cache. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- If this instruction has UB address overlap with other instructions, you must insert the synchronization instructions [asc_sync_notify](../sync/intra_core_sync/asc_sync_notify.md) and [asc_sync_wait](../sync/intra_core_sync/asc_sync_wait.md) to serialize multiple instructions and prevent abnormal data.
- The address range width of all data that one instruction can obtain must not exceed 40 bits (1TB). That is, the i-dimension size of the source operand is (loopi_lp_size + loopi_size + loopi_rp_size - 1) * loop0_src_stride, and the i-dimension size of the destination operand is (loopi_lp_size + loopi_size + loopi_rp_size - 1) * loop0_dst_stride. The sum of the sizes of all dimensions must not exceed 2^40 bits.
- When the loopi_dst_stride of each dimension is an ascending sequence, the address spaces between different loops must not interleave or overlap.

## Examples

2D Padding scenario, using the fixed value filling mode
![ ](../../../figures/asc_ndim_copy_gm2ub_2d_padding_constant.png)

```cpp
// total_length refers to the total length of the data to be moved. src is the half-type GM memory input externally.
constexpr uint64_t total_length = 54;
__ubuf__ half dst[total_length];

uint32_t loop0_size = 3;
uint32_t loop1_size = 4;
uint32_t loop2_size = 1;
uint32_t loop3_size = 1;
uint32_t loop4_size = 1;
uint64_t loop0_src_stride = 1;
uint64_t loop1_src_stride = 3;
uint64_t loop2_src_stride = 0;
uint64_t loop3_src_stride = 0;
uint64_t loop4_src_stride = 0;
uint64_t loop0_dst_stride = 1;
uint64_t loop1_dst_stride = 6;
uint64_t loop2_dst_stride = 0;
uint64_t loop3_dst_stride = 0;
uint64_t loop4_dst_stride = 0;

bool padding_mode = true;
half pad_value = 88.0;
uint8_t loop0_lp_count = 2;
uint8_t loop1_lp_count = 2;
uint8_t loop0_rp_count = 1;
uint8_t loop1_rp_count = 3;

asc_set_ndim_loop0_stride(loop0_dst_stride, loop0_src_stride);
asc_set_ndim_loop1_stride(loop1_dst_stride, loop1_src_stride);
asc_set_ndim_loop2_stride(loop2_dst_stride, loop2_src_stride);
asc_set_ndim_loop3_stride(loop3_dst_stride, loop3_src_stride);
asc_set_ndim_loop4_stride(loop4_dst_stride, loop4_src_stride);

asc_set_ndim_pad_value(pad_value);

asc_ndim_pad_count_config pad_count_config;
pad_count_config.loop1_lp_count = loop1_lp_count;
pad_count_config.loop1_rp_count = loop1_rp_count;
asc_set_ndim_pad_count(pad_count_config);

asc_ndim_copy_gm2ub(dst, src, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
    loop0_lp_count, loop0_rp_count, padding_mode, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);
```

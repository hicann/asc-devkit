# asc_copy_gm2ub_align

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-08T04:24:34.824Z pushedAt=2026-09-08T13:17:42.508Z -->

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

Provides non-aligned data copy from Global Memory (GM) to Unified Buffer (UB), and supports 8-bit/16-bit/32-bit data type copy.

When performing loop copy, set the loop mode through [asc_set_gm2ub_loop_size](../asc_set_gm2ub_loop_size.md), [asc_set_gm2ub_loop1_stride](../asc_set_gm2ub_loop1_stride.md), and [asc_set_gm2ub_loop2_stride](../asc_set_gm2ub_loop2_stride.md).

## Prototype

- Copies the first n data items

```cpp
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size)

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint32_t size)
```

- High-dimensional split copy

```cpp
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
```

- Synchronous computation

```cpp
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size)

__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ half* dst, __gm__ half* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ float* dst, __gm__ float* src, uint32_t size)
```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Starting address of the destination operand in UB. |
| src | Input | Starting address of the source operand in GM. |
| size | Input | Size of the data to be copied (in bytes). |
| n_burst | Input | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| len_burst | Input | Length of each consecutive data block to be copied, in bytes. Value range: [1, 2097151]. |
| left_padding_num | Input | Data range to be padded on the left side of the consecutive data blocks, in number of elements. The byte count of left_padding_num cannot exceed 32.|
| right_padding_num | Input | Data range to be padded on the right side of the consecutive data blocks, in number of elements. The byte count of right_padding_num cannot exceed 32.|
| enable_constant_pad | Input | Configures whether the non-aligned padding value is a constant. Value description:  <br>&bull; **true**: The padding value is the value set by [asc_set_copy_pad_val](../asc_set_copy_pad_val.md). <br>&bull; **false**: The padding value is the first value of the input data. |
| l2_cache_mode | Input | Configures the data management policy in the L2 Cache. The values are described as follows:  <br>&bull; 0: DISABLE mode, suitable for data that is accessed only once. <br>&bull; 1: NORMAL mode, suitable for data whose reuse pattern is unknown or not extreme. <br>&bull; 2: LAST mode, suitable for data that is accessed frequently and repeatedly. <br>&bull; 4: PERSISTENT mode, suitable for data that needs to reside in the cache for a long time. |
| src_stride | Input | Interval between adjacent consecutive data blocks of the source operand (the interval between the head of the preceding data block and the head of the following data block).<br>The unit is bytes. |
| dst_stride | Input | Interval between adjacent consecutive data blocks of the destination operand (the interval between the head of the preceding data block and the head of the following data block).<br>The unit is bytes. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- The start address of **src** must be aligned to the byte size of the corresponding data type.
- The start address of **dst** must be 32-byte aligned.
- If multiple **asc_copy_gm2ub_align** instructions need to be executed and their destination addresses overlap, insert synchronization instructions ([asc_sync_notify](../../sync/asc_sync_notify.md) and [asc_sync_wait](../../sync/asc_sync_wait.md)) to ensure serialization of multiple **asc_copy_gm2ub_align** instructions and prevent abnormal data.
- The byte count of **left_padding_num** and **right_padding_num** must not exceed 32 bytes.
- When the first n data items are copied, the size of the copied data must be 32-byte aligned.
- When **dst_stride** is not equal to **len_burst**, **dst_stride** requires 32-byte alignment.

## Example

```cpp
asc_set_gm2ub_loop_size(2, 2);
asc_set_gm2ub_loop1_stride(96, 128);
asc_set_gm2ub_loop2_stride(192, 288);
asc_copy_gm2ub_align(dst, src, 2, 48 * sizeof(int8_t), 0, 0, false, 0, 48 * sizeof(int8_t), 48 * sizeof(int8_t));
asc_set_gm2ub_loop_size(1, 1);
```

# asc_copy_ub2gm_align

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-08T04:32:41.267Z pushedAt=2026-09-08T13:17:42.517Z -->

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

Copies data from Unified Buffer (UB) to Global Memory (GM), supporting 8-bit/16-bit/32-bit data type transfer.

For loop transfer, set the loop mode through [asc_set_ub2gm_loop_size](../asc_set_ub2gm_loop_size.md), [asc_set_ub2gm_loop1_stride](../asc_set_ub2gm_loop1_stride.md), and [asc_set_ub2gm_loop2_stride](../asc_set_ub2gm_loop2_stride.md), and use them together with this API.

## Prototype

- Copies the first n data items

```cpp
__aicore__ inline void asc_copy_ub2gm_align(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size)

__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ half* dst, __ubuf__ half* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ float* dst, __ubuf__ float* src, uint32_t size)
```

- High-dimensional split copy

```c++
__aicore__ inline void asc_copy_ub2gm_align(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ half* dst, __ubuf__ half* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
__aicore__ inline void asc_copy_ub2gm_align(__gm__ float* dst, __ubuf__ float* src, uint16_t n_burst, uint32_t len_burst, uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
```

- Synchronous computation

```cpp
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size)

__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ half* dst, __ubuf__ half* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ float* dst, __ubuf__ float* src, uint32_t size)
```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Starting address of the destination operand in GM. |
| src | Input | Starting address of the source operand in UB. |
| size | Input | Size of the data to be copied (in bytes). |
| n_burst | Input | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| len_burst | Input | Length of each consecutive data block to be transferred, in bytes. Value range: [1, 2097151]. |
| l2_cache_mode | Input | Configures the data management policy in the L2 Cache. The values are described as follows:  <br>&bull; 0: DISABLE mode, suitable for data that is accessed only once. <br>&bull; 1: NORMAL mode, suitable for data whose reuse pattern is unknown or not extreme. <br>&bull; 2: LAST mode, suitable for data that is accessed frequently and repeatedly. <br>&bull; 4: PERSISTENT mode, suitable for data that needs to reside in the cache for a long time. |
| dst_stride | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the head of the preceding data block and the head of the following data block).<br>The unit is bytes. |
| src_stride | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the head of the preceding data block and the head of the following data block).<br>The unit is bytes. |

## Return Value

None

## Pipeline Type

PIPE_MTE3

## Constraints

- The start address of **dst** must be aligned to the byte size of the corresponding data type.
- The start address of **src** must be 32-byte aligned.
- If multiple **asc_copy_ub2gm_align** instructions need to be executed and their destination addresses overlap, insert synchronization instructions ([asc_sync_notify](../../sync/asc_sync_notify.md) and [asc_sync_wait](../../sync/asc_sync_wait.md)) to serialize the multiple **asc_copy_ub2gm_align** instructions and prevent abnormal data.
- When the first n data items are copied, the size of the copied data must be 32-byte aligned.
- When **src_stride** is not equal to **len_burst**, **src_stride** must be 32-byte aligned.

## Example

```cpp
uint32_t loop1_size = 2;
uint32_t loop2_size = 2;
uint64_t loop1_src_stride = 96;
uint64_t loop1_dst_stride = 128;
uint64_t loop2_src_stride = 192;
uint64_t loop2_dst_stride = 288;
asc_set_ub2gm_loop_size(loop1_size, loop2_size);
asc_set_ub2gm_loop1_stride(loop1_src_stride, loop1_dst_stride);
asc_set_ub2gm_loop2_stride(loop2_src_stride, loop2_dst_stride);
asc_copy_ub2gm_align(dst, src, 2, 48 * sizeof(int8_t), 0, 48 * sizeof(int8_t), 48 * sizeof(int8_t));
asc_set_ub2gm_loop_size(1, 1);
```

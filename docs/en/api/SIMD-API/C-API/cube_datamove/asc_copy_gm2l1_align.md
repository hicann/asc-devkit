# asc_copy_gm2l1_align

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:39:34.614Z -->

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

Performs real-time padding on matrix data and, after padding, copies the data from Global Memory to the L1 Buffer. It must be used together with [asc_set_gm2l1_loop_size](./asc_set_gm2l1_loop_size.md), [asc_set_gm2l1_loop1_stride](./asc_set_gm2l1_loop1_stride.md), [asc_set_gm2l1_loop2_stride](./asc_set_gm2l1_loop2_stride.md), and [asc_set_gm2l1_pad](./asc_set_gm2l1_pad.md).

## Prototype

- Regular copy

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ half* dst, __gm__ half* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align(__cbuf__ float* dst, __gm__ float* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    ```

- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ int16_t* dst, __gm__ int16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ half* dst, __gm__ half* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count,uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    __aicore__ inline void asc_copy_gm2l1_align_sync(__cbuf__ float* dst, __gm__ float* src, uint32_t n_burst, uint32_t len_burst, uint8_t left_padding_count, uint8_t right_padding_count, bool data_select_bit, uint8_t l2_cache_ctl, uint64_t burst_src_stride, uint32_t burst_dst_stride)
    ```

## Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| n_burst | Input | Number of consecutive data blocks to be copied. |
| len_burst | Input | Length of each consecutive data block to be copied, in units of 32 bytes. For the 16-bit data type, this parameter must be a multiple of 2. For the 32-bit data type, this parameter must be a multiple of 4. |
| left_padding_count | Input | Number of padding elements on the left side of the data. For the 8-bit data type, the maximum value of this parameter is 32. For the 16-bit data type, the maximum value of this parameter is 16. For the 32-bit data type, the maximum value of this parameter is 8. |
| right_padding_count | Input | Number of padding elements on the right side of the data. For the 8-bit data type, the maximum value of this parameter is 32. For the 16-bit data type, the maximum value of this parameter is 16. For the 32-bit data type, the maximum value of this parameter is 8. |
| data_select_bit | Input | Padding data selection bit. Currently, only setting it to **false** is supported, which sets the padding value to the first element of the data block. |
| l2_cache_ctl | Input | Configures the management policy of the data in L2 Cache. The values of this parameter are described as follows: <br>&bull; **0**: DISABLE mode, suitable for data that is accessed only once. <br>&bull; **1**: NORMAL mode, suitable for data whose reuse pattern is unknown or not extreme. <br>&bull; **2**: LAST mode, suitable for data that is accessed frequently and repeatedly. <br>&bull; **4**: PERSISTENT mode, suitable for data that needs to reside in the cache for a long time. |
| burst_src_stride | Input | Distance between the heads of consecutive data blocks corresponding to two adjacent bursts in the input data. |
| burst_dst_stride | Input | Distance between the heads of consecutive data blocks corresponding to two adjacent bursts in the output data. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- The destination operands must not have overlapping addresses. For operations with overlapping addresses, the hardware returns no warning or error, and the correctness of the data in the overlapping address range is not guaranteed.

## Example

```cpp
// The number of consecutive data blocks to be copied is 2.
constexpr uint32_t n_burst = 2;
// The length of each consecutive data block to be copied is 64 bytes.
constexpr uint32_t len_burst = 2;
// The number of padding elements on the left and right sides of the data is 0.
constexpr uint8_t left_padding_count = 0;
constexpr uint8_t right_padding_count = 0;
// The padding value takes the first element of the data block.
constexpr bool data_select_bit = false;
// The L2 cache uses the DISABLE mode.
constexpr uint8_t l2_cache_ctl = 0;
// The distance between two adjacent consecutive data blocks in the input and output data is 0.
constexpr uint64_t burst_src_stride = 0;
constexpr uint32_t burst_dst_stride = 0;
__gm__ half src[256];
__cbuf__ half dst[256];
asc_copy_gm2l1_align(dst, src, n_burst, len_burst,  left_padding_count, right_padding_count, data_select_bit, l2_cache_ctl, burst_src_stride, burst_dst_stride);
```
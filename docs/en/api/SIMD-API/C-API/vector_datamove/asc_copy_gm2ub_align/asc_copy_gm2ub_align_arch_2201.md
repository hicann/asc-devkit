# asc_copy_gm2ub_align

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-08T04:21:08.175Z pushedAt=2026-09-08T13:17:42.507Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
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

Provides unaligned data transfer, moving data from Global Memory (GM) to Unified Buffer (UB), and supports 8-bit/16-bit/32-bit data type transfer.

## Prototype

- Copies the first n data items

    ```cpp
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
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
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    __aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint16_t n_burst, uint32_t len_burst, uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
    __aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
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
| **dst** | Output | Start address of the destination UB. |
| **src** | Input | Start address of the source GM. |
| **size** | Input | Size of the data to be copied (in bytes). |
| **n_burst** | Input | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| **len_burst** | Input | Length of each consecutive data block to be transferred, in bytes. Value range: [1, 2097151]. |
| **left_padding_num** | Input | Data range to be padded on the left side of the consecutive data blocks, in elements. The byte size of **left_padding_num** cannot exceed 32. |
| **right_padding_num** | Input | Data range to be padded on the right side of the consecutive data blocks, in elements. The byte size of **right_padding_num** cannot exceed 32. |
| **src_gap** | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the tail of the preceding data block and the head of the following data block).<br>The unit is DataBlock (32 bytes). |
| **dst_gap** | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the end of the preceding data block and the start of the following data block).<br>The unit is DataBlock (32 bytes). |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- The start address of **src** must be aligned to the byte size of the corresponding data type.
- The start address of **dst** must be 32-byte aligned.
- If multiple **asc_copy_gm2ub_align** instructions need to be executed and their destination addresses overlap, a synchronization instruction must be inserted to ensure serialization of the multiple **asc_copy_gm2ub_align** instructions and prevent abnormal data.
- Synchronous computation includes synchronization wait.
- The byte size of **left_padding_num** and **right_padding_num** must not exceed 32 bytes.

## Example

```cpp
// total_length refers to the total length of the data involved in the transfer. src is the GM memory input from outside.
constexpr uint64_t total_length = 128;
__ubuf__ uint8_t dst[total_length];
asc_copy_gm2ub_align(dst, src, total_length * sizeof(uint8_t));
```

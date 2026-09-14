# asc_copy_gm2l1

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-28T10:10:43.082Z pushedAt=2026-08-29T07:20:45.349Z -->

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

Copies matrix data from Global Memory to the L1 Buffer and supports different pad modes.

## Prototype

- Normal copy mode

    ```cpp
    // Do not perform the pad operation.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Insert a 31B pad for every 1B, in units of 32B.
    __aicore__ inline void asc_copy_gm2l1_pad1(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Insert a 15B pad for every 1B, in units of 32B.
    __aicore__ inline void asc_copy_gm2l1_pad2(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Insert a 14B pad for every 2B, in units of 32B.
    __aicore__ inline void asc_copy_gm2l1_pad3(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Insert a 12B pad for every 4B, in units of 32B.
    __aicore__ inline void asc_copy_gm2l1_pad4(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Insert an 8B pad for every 8B, in units of 32B.
    __aicore__ inline void asc_copy_gm2l1_pad5(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Delete the highest 28B of every 32B and store the lowest 4B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad6(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Delete the highest 24B of every 32B and store the lowest 8B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad7(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Delete the highest 16B of every 32B and store the lowest 16B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad8(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    ```

- Normal copy mode (high-dimensional slicing)

    ```cpp
    // Do not perform pad operation.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // In units of 32B, insert 31B of pad for every 1B.
    __aicore__ inline void asc_copy_gm2l1_pad1(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // In units of 32B, insert 15B of pad for every 1B.
    __aicore__ inline void asc_copy_gm2l1_pad2(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // In units of 32B, insert 14B of pad for every 2B.
    __aicore__ inline void asc_copy_gm2l1_pad3(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // In units of 32B, insert 12B of pad for every 4B.
    __aicore__ inline void asc_copy_gm2l1_pad4(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // In units of 32B, insert 8B of pad for every 8B.
    __aicore__ inline void asc_copy_gm2l1_pad5(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // Delete the highest 28B of every 32B and store the lowest 4B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad6(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // For every 32B, remove the highest 24B and store the lowest 8B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad7(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    // For every 32B, remove the highest 16B and store the lowest 16B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad8(__cbuf__ void* dst, __gm__ void* src, uint16_t n_burst,
                                      uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```

- Normal copy mode (synchronous)

    ```cpp
    // No padding is performed.
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, insert 31B of padding for every 1B.
    __aicore__ inline void asc_copy_gm2l1_pad1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, insert 15B of padding for every 1B.
    __aicore__ inline void asc_copy_gm2l1_pad2_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, insert 14B of padding for every 2B.
    __aicore__ inline void asc_copy_gm2l1_pad3_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, insert 12B of padding for every 4B.
    __aicore__ inline void asc_copy_gm2l1_pad4_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // In units of 32B, insert 8B of padding for every 8B.
    __aicore__ inline void asc_copy_gm2l1_pad5_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // For every 32B, remove the highest 28B and store the lowest 4B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad6_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Delete the highest 24B of every 32B, and store the lowest 8B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad7_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    // Delete the highest 16B of every 32B, and store the lowest 16B contiguously.
    __aicore__ inline void asc_copy_gm2l1_pad8_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
    ```

- 2D copy mode

    ```cpp
    // 2D copy of bfloat16_t matrix data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy of half matrix data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy of float matrix data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy of int32_t matrix data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy of int8_t matrix data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy of uint32_t matrix data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy of uint8_t matrix data.
    __aicore__ inline void asc_copy_gm2l1(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    ```

- 2D copy mode (synchronous)

    ```cpp
    // 2D copy bfloat16_t matrix data (sync).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy half matrix data (sync).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy float matrix data (sync).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy int32_t matrix data (sync).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy int8_t matrix data (sync).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy uint32_t matrix data (sync).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    // 2D copy uint8_t matrix data (sync).
    __aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
    ```

## Parameters

### Normal Copy Mode Parameters

| Parameter  | Input/Output | Description                                                              |
|:----------|:------|:----------------------------------------------------------------|
| dst       | Output    | Start address of the destination operand (vector).                                                 |
| src       | Input    | Start address of the source operand.                                                      |
| size      | Input    | Size of the data to be copied (in bytes).                                                     |
| n_burst   | Input    | Number of consecutive data blocks to be copied. Value range: [1, 4095].                                   |
| len_burst | Input    | Length of each consecutive data block to be copied, in DataBlock (32 bytes). Value range: [1, 65535].            |
| src_gap   | Input    | Gap between adjacent consecutive data blocks of the source operand (the gap between the end of the preceding data block and the start of the following data block).<br>Unit: DataBlock (32 bytes).  |
| dst_gap   | Input    | Gap between adjacent consecutive data blocks of the destination operand (the gap between the end of the preceding data block and the start of the following data block).<br>Unit: DataBlock (32 bytes). |

### 2D Copy Mode Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (vector). |
| src | Input | Start address of the source operand (vector). |
| base_idx | Input | Fractal matrix ID, indicating that the copy starts from the specified fractal in the source operand (0 indicates the first fractal matrix in the source operand). Value range: [0, 65535]. Unit: 512B. Default value: 0. |
| repeat | Input | Number of iterations. |
| src_stride | Input | Interval between the start addresses of the previous and next fractals of the source operand across adjacent iterations. Unit: 512B. Value range: [0, 65535]. Default value: 0. |
| dst_gap | Input | Interval between the end address of the previous fractal and the start address of the next fractal of the destination operand across adjacent iterations. Unit: 512B. Value range: [0, 65536]. Default value: 0. |

## Return Value

None

## Pipeline Type

PIPE_MTE2

## Constraints

- The start address of **src** must be aligned to the byte size of the corresponding data type.
- The start address of **dst** must be 32-byte aligned.
- If multiple **asc_copy_gm2l1** instructions need to be executed and their destination addresses overlap, a synchronization instruction must be inserted to ensure serialization of the multiple **asc_copy_gm2l1** instructions and prevent abnormal data.

## Example

```cpp
// Normal data copy mode
//total_length indicates the total length of the data involved in the copy.
constexpr uint64_t total_length = 128;
__cbuf__ half dst[total_length];
__gm__ half src[total_length];
// Number of data blocks copied consecutively
constexpr uint16_t n_burst = 1;
// Length of the data block copied consecutively, which is 512 half elements here
constexpr uint16_t len_burst = 512 * sizeof(half) / 32;
// Copy the source operand consecutively
constexpr uint16_t src_gap = 0;
// Arrange the destination operand consecutively
constexpr uint16_t dst_gap = 0;
asc_copy_gm2l1((__cbuf__ void*)dst, (__gm__ void*)src, n_burst, len_burst, src_gap, dst_gap);

// 2D data copy mode
//The starting block of the copy is 1.
constexpr uint16_t base_idx = 1;
//The number of copy iterations is 2.
constexpr uint8_t repeat = 2;
//The input copy stride is 0 bytes, and the output copy stride is 1024 bytes.
constexpr uint16_t src_stride = 0;
constexpr uint16_t dst_gap = 1;
__gm__ half src[256];
__cbuf__ half dst[256];
asc_copy_gm2l1(dst, src, base_idx, repeat, src_stride, dst_gap);
```

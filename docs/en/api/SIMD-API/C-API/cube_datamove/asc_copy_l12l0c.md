# asc_copy_l12l0c

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:46:23.451Z -->

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

Copies a matrix from the L1 Buffer to the L0C Buffer. The data types supported by this copy operation are as follows:
- bfloat16_t->bfloat16_t
- half->half
- float->half
- float->bfloat16_t
- float->float
- int32_t->int32_t
- uint32_t->uint32_t

## Prototype

- Regular copy

    ```cpp
    __aicore__ inline void asc_copy_l12l0c(__cc__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ half* dst, __cbuf__ half* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ half* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ bfloat16_t* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ float* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ int32_t* dst, __cbuf__ int32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c(__cc__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```
- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ half* dst, __cbuf__ half* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ half* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ bfloat16_t* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ float* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ int32_t* dst, __cbuf__ int32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12l0c_sync(__cc__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```

## Parameters


| Parameter | Input/Output | Description                                                              |
| :--- |:------|:----------------------------------------------------------------|
| **dst** | Output    | Start address of the destination operand. |
| **src** | Input    | Start address of the source operand. |
| **n_burst** | Input    | Number of consecutive data copy blocks to be moved. Value range: [1, 4095]. |
| **len_burst** | Input    | Length of each consecutive data copy block to be moved, in units of 32 bytes. Value range: [1, 65535]. |
| **src_gap** | Input    | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the source operand. |
| **dst_gap** | Input    | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- The start address of **src** must be aligned to the byte size of the corresponding data type.
- The start address of **dst** must be 32-byte aligned.
- If multiple **asc_copy_l12l0c** instructions need to be executed and their destination addresses overlap, a synchronization instruction must be inserted to ensure serialization of the multiple **asc_copy_l12l0c** instructions and prevent abnormal data.

## Example

```cpp
// The number of consecutive data copy blocks is 1.
constexpr uint16_t n_burst = 1;
// The length of each consecutive data copy block is 32 bytes.
constexpr uint16_t len_burst = 1;
// Between adjacent iterations, the gap between the start addresses of the previous and next fractals of the source operand is 64 bytes.
constexpr uint16_t src_gap = 1;
// Between adjacent iterations, the gap between the start addresses of the previous and next fractals of the destination operand is 32 bytes.
constexpr uint16_t dst_gap = 0;
__cbuf__ half src[256];
__cc__ half dst[256];
asc_copy_l12l0c(dst, src, n_burst, len_burst, src_gap, dst_gap);
```

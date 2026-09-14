# asc_copy_l12gm

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:41:10.283Z -->

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
- Atlas inference product AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference product Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training product: Not supported
<!-- end id7 -->

## Description

Copies data from the L1 Buffer to Global Memory.

## Prototype

- Regular copy

    ```cpp
    __aicore__ inline void asc_copy_l12gm(__gm__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```
- Synchronous copy

    ```cpp
    __aicore__ inline void asc_copy_l12gm_sync(__gm__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```

## Parameters


| Parameter | Input/Output | Description                                                              |
| :--- |:------|:----------------------------------------------------------------|
| **dst** | Output    | Start address of the destination operand. |
| **src** | Input    | Start address of the source operand. |
| **n_burst** | Input    | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| **len_burst** | Input    | Length of each consecutive data block to be copied, in units of 32 bytes. Value range: [1, 65535]. |
| **src_gap** | Input    | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the source operand. |
| **dst_gap** | Input    | Gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand, in units of 32 bytes. |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- The start address of **src** must be aligned to the number of bytes occupied by the corresponding data type.
- The start address of **dst** must be 32-byte aligned.
- If multiple **asc_copy_l12gm** instructions need to be executed and their destination addresses overlap, a synchronization instruction must be inserted to ensure that the multiple **asc_copy_l12gm** instructions are serialized, preventing abnormal data.

## Example

```cpp
// The number of consecutive data blocks to be copied is 1.
constexpr uint16_t n_burst = 1;
// The length of each consecutive data block to be copied is 32 bytes.
constexpr uint16_t len_burst = 1;
// The gap between the start addresses of the previous and next fractals of the source operand in adjacent iterations is 32 bytes.
constexpr uint16_t src_gap = 0;
// The gap between the start addresses of the previous and next fractals of the destination operand in adjacent iterations is 64 bytes.
constexpr uint16_t dst_gap = 1;
__cbuf__ half src[256];
__gm__ half dst[256];
asc_copy_l12gm(dst, src, n_burst, len_burst, src_gap, dst_gap);
```

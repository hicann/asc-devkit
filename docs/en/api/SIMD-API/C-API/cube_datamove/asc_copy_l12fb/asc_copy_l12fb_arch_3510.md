# asc_copy_l12fb

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-28T10:20:18.227Z pushedAt=2026-08-29T07:21:12.995Z -->

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

Copies data from the L1 Buffer to the Fixpipe Buffer, which is used to store quantization parameters.

## Prototype

- Copies the first n data items
    ```cpp
    __aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size)
    ```

- High-dimensional split copy
    ```cpp
    __aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap_size, uint16_t dst_gap_size)
    ```

- Synchronous copy
    ```cpp
    __aicore__ inline void asc_copy_l12fb_sync(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size)
    ```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand. |
| src | Input | Start address of the source operand. |
| size | Input | Size of the data to be copied (in bytes). |
| n_burst | Input | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| len_burst | Input | Length of each consecutive data block to be copied, with the unit being 64B. Value range: [1, 65535]. |
| src_gap_size | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the tail of the preceding data block and the head of the following data block).<br>The unit is DataBlock (32 bytes). |
| dst_gap_size | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the tail of the preceding data block and the head of the following data block).<br>The unit is 64B. |

## Return Value

None

## Pipeline Type

PIPE_FIX

## Constraints

- The start addresses of **dst** and **src** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).

## Example

```cpp
constexpr uint16_t n_burst = 1;
constexpr uint16_t len_burst = 1;
constexpr uint16_t src_gap = 0;
constexpr uint16_t dst_gap = 1;
__cbuf__ half src[256];
__fbuf__ half dst[256];
asc_copy_l12fb(dst, src, n_burst, len_burst, src_gap, dst_gap);
```

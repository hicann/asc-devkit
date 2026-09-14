# asc_copy_l12bt

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-28T10:16:52.623Z pushedAt=2026-08-29T07:21:06.790Z -->

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

Copies the Bias data of the MMAD instruction from the L1 Buffer to the BiasTable Buffer.

## Prototype

- Copies the first n data items
    ```cpp
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ void* src, uint32_t size)
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ bfloat16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ half* src, uint32_t size)
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ float* src, uint32_t size)
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ int32_t* src, uint32_t size)
    ```

- High-dimensional split copy
    ```cpp
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ void* src, uint16_t conv_control, uint16_t n_burst, uint16_t len_burst, uint16_t source_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ bfloat16_t* src, uint16_t conv_control, uint16_t n_burst, uint16_t len_burst, uint16_t source_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ half* src, uint16_t conv_control, uint16_t n_burst, uint16_t len_burst, uint16_t source_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ float* src, uint16_t conv_control, uint16_t n_burst, uint16_t len_burst, uint16_t source_gap, uint16_t dst_gap)
    __aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ int32_t* src, uint16_t conv_control, uint16_t n_burst, uint16_t len_burst, uint16_t source_gap, uint16_t dst_gap)
    ```

- Synchronous copy
    ```cpp
    __aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ void* src, uint32_t size)
    __aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ bfloat16_t* src, uint32_t size)
    __aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ half* src, uint32_t size)
    __aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ float* src, uint32_t size)
    __aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ int32_t* src, uint32_t size)
    ```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand. |
| src | Input | Start address of the source operand. |
| size| Input | Size of the data to be copied (in bytes).|
| conv_control | Input | Whether to enable in-place conversion.|
| n_burst | Input | Number of consecutive data blocks to be copied. Value range: [1, 4095]. |
| len_burst | Input | Length of each consecutive data block to be copied, in DataBlock (32 bytes). Value range: [1, 65535]. |
| source_gap | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the end of the preceding data block and the start of the following data block).<br>Unit: DataBlock (32 bytes). |
| dst_gap | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the end of the preceding data block and the start of the following data block).<br>Unit: DataBlock (32 bytes). |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- The start addresses of **dst** and **src** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- When the API for moving the first n data blocks is used, the size of the data to be copied must be 32-byte aligned.

## Example

```cpp
constexpr uint16_t n_burst = 1;
constexpr uint16_t len_burst = 1;
constexpr uint16_t source_gap = 0;
constexpr uint16_t dst_gap = 1;
constexpr uint16_t conv_control = 0;
__cbuf__ half src[256];
__biasbuf__ half dst[256];
uint64_t dst_addr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst)) / 16;
asc_copy_l12bt(dst_addr, src, conv_control, n_burst, len_burst, source_gap, dst_gap);
```

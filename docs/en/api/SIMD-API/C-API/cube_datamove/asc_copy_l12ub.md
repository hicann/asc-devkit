# asc_copy_l12ub

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:47:04.924Z -->

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

Copies data from the **L1 Buffer** to the **Unified Buffer**.

## Prototype

- High-dimensional split copy
    ```cpp
    __aicore__ inline void asc_copy_l12ub(__ubuf__ void* dst_addr, __cbuf__ void* src_addr, bool sub_blockid, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```

- Synchronous copy
    ```cpp
    __aicore__ inline void asc_copy_l12ub_sync(__ubuf__ void* dst_addr, __cbuf__ void* src_addr, bool sub_blockid, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
    ```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst_addr | Output | Start address of the destination operand. |
| src_addr | Input | Start address of the source operand. |
| sub_blockid | Input | Sub-block ID used. |
| n_burst | Input | Number of consecutive data blocks to be copied. |
| len_burst | Input | Length of each consecutive data block to be copied. The unit is **DataBlock** (32 bytes). |
| src_gap | Input | Gap between adjacent consecutive data blocks of the source operand (the gap between the tail of the preceding data block and the head of the following data block).<br>The unit is **DataBlock** (32 bytes). |
| dst_gap | Input | Gap between adjacent consecutive data blocks of the destination operand (the gap between the tail of the preceding data block and the head of the following data block).<br>The unit is **DataBlock** (32 bytes). |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- The start addresses of **dst_addr** and **src_addr** must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- Because this API implements data copy between the Cube core and the Vector core, if a kernel function calls this API, the `__cube__` or `__vector__` execution space modifier cannot be used; otherwise, execution fails.

## Example

```cpp
constexpr uint16_t n_burst = 1;
constexpr uint16_t len_burst = 1;
constexpr uint16_t src_gap = 0;
constexpr uint16_t dst_gap = 1;
__cbuf__ half src[256];
__ubuf__ half dst[256];
asc_copy_l12ub(dst, src, false, n_burst, len_burst, src_gap, dst_gap);
```

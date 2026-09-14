# asc_frequency_histogram

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-04T09:49:45.559Z pushedAt=2026-09-05T12:14:28.834Z -->

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

Performs frequency statistics on histogram data. Based on the low-bit/high-bit mode, it counts the data in the [0, 127] and [128, 255] ranges respectively. The n-th bit in **dst** represents the frequency of n appearing in **src**, and the counted data is accumulated on top of the source data in **dst**.

The Vector Length is 256 bytes, and the **dst** data type is **uint16_t**. One **dst** can store 128 data entries, so two **dst** are required. BIN0 indicates the low-bit mode, which counts and writes the data in the [0, 127] range of **src**. BIN1 indicates the high-bit mode, which counts and writes the data in the [128, 255] range of **src**.

The statistics method is shown in the following figure, where dst0 indicates the low-bit mode and dst1 indicates the high-bit mode.

![frequency statistics](../../figures/frequency_statistics.png)

## Prototype

  ```cpp
  // Low-bit mode.
  __simd_callee__ inline void asc_frequency_histogram_bin0(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
  // High-bit mode.
  __simd_callee__ inline void asc_frequency_histogram_bin1(vector_uint16_t& dst, vector_uint8_t src, vector_bool mask)
  ```

## Parameters

| Parameter       | Input/Output | Description                |
| --------- | ----- | ----------------- |
| dst       | Output    | Destination operand (vector data register). |
| src       | Input    | Source operand (vector data register). |
| mask      | Input    | Source operand mask (mask register), which indicates which elements participate in the computation. A value of 1 at the corresponding position means the element participates in the computation, and 0 means it does not. Elements not selected by the mask are set to zero in the output. |

For details about vector data registers and mask registers, see [reg Data Definition](../reg_data_definition.md).

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

None

## Example

```cpp
vector_uint16_t dst;
vector_uint8_t src;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr is the externally input UB memory space address.
asc_frequency_histogram_bin0(dst, src, mask);
```

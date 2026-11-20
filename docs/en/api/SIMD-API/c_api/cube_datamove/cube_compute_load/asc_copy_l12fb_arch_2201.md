---
okftype: reference.api
description: "Moves the accompanying quantization parameters in the L1 Buffer into the Fixpipe Buffer, providing two forms: continuous copy by size, and high-dimensional splitting by data block count, block length, and source/destination interval, for subsequent reading when L0C moves out."
tag:
  - roles: [data_movement]
  - operations: [copy]
  - traits: [aligned_transfer, datablock_granularity]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: requires
    condition: "After moving in, configure the start address index of the quantization parameter region"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization.md
    kind: related
version: ">=9.0.0"
version_changes: []
---
# asc_copy_l12fb (NPU Architecture Version 2201)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:39:04.211Z pushedAt=2026-11-18T08:15:34.266Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
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

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Moves data from the L1 Buffer to the Fixpipe Buffer, where the Fixpipe Buffer is used to store quantization parameters.

For the encoding of each column of quantization parameters and the Scalar/Vector distinction, see [Accompanying Quantization](../cube_store_key_features/accompanying_quantization.md). After moving the parameters in, configure the parameter region start address index through [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md), and then call the L0C move-out interface of the current architecture. If activation is also enabled, see [Accompanying Quantization and ReLU Scenario Combination](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md).

## Prototype

- Move the first n data elements

    ```cpp
    __aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size)
    ```

- High-dimensional split move

    ```cpp
    __aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap_size, uint16_t dst_gap_size)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Start address of the destination operand. |
| src | Input | Start address of the source operand. |
| size | Input | Size of the data to move (in bytes). |
| n_burst | Input | Number of consecutive transfer data blocks to move. Value range: [1, 4095]. |
| len_burst | Input | Length of each consecutive transfer data block to move, in DataBlock (32 bytes). Value range: [1, 65535]. |
| src_gap_size | Input | Interval between adjacent consecutive data blocks of the source operand (the interval between the tail of the preceding data block and the head of the following data block).<br>Unit: DataBlock (32 bytes). |
| dst_gap_size | Input | Interval between adjacent consecutive data blocks of the destination operand (the interval between the tail of the preceding data block and the head of the following data block).<br>Unit: DataBlock (32 bytes). |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- When the first n data transfer interfaces are used, the transferred data size must be 32-byte aligned.

## Examples

```cpp
constexpr uint16_t n_burst = 1;
constexpr uint16_t len_burst = 1;
constexpr uint16_t src_gap_size = 0;
constexpr uint16_t dst_gap_size = 1;
__cbuf__ half src[256];
__fbuf__ half dst[256];
asc_copy_l12fb(dst, src, n_burst, len_burst, src_gap_size, dst_gap_size);
```

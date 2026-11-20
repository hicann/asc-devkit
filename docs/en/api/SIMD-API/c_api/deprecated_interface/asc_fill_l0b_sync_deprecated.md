---
okftype: reference.api
description: "Deprecated synchronous interface for L0B Buffer initialization. It fills the binary value of the scalar value into L0B according to repeat, blk_num, and dst_gap of asc_fill_value_config, supporting two input types: half and uint32_t. The replacement is asc_fill_l0b combined with asc_sync."
tag:
  - roles: [data_initialization]
  - operations: [fill]
  - traits: [scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_fill_l0b.md
    kind: replaced_by
    condition: "The asynchronous fill interface replaces the synchronous interface on this page"
  - path: docs/zh/api/SIMD-API/c_api/defs/union/asc_fill_value_config.md
    kind: requires
    condition: "Initialization parameters such as repeat, blk_num, and dst_gap are carried by this structure"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync.md
    kind: requires
    condition: "After switching to the non-synchronous interface, you must call asc_sync yourself to complete synchronization"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The include header uniformly marks all 15 overloads with ASC_DEPRECATED(9.2.0, 2027/09/07, asc_fill_l0b and asc_sync); the CANN_9_2_0 Release Notes deprecation table lists the asc_fill_*_sync family; the interface is retained and available only on 2201."
---
# asc_fill_l0b_sync (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:59:31.330Z pushedAt=2026-11-18T03:12:16.483Z nl=2 -->

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

The header file path is: `"c_api/composite/cube_datamove_composite.h"`.

**The `asc_fill_l0b_sync` API is deprecated. Use the [corresponding asynchronous API](../cube_datamove/cube_load_aux_config/asc_fill_l0b.md) and the synchronization API [asc_sync](../sync/intra_core_sync/asc_sync.md) instead.**

Assigns the binary value of value to dst to initialize the Local Memory of the L0B Buffer.

## Prototype

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_fill_l0b_sync(__cb__ int16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ int16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ uint16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ half* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ half* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ int32_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ int32_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ uint32_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ float* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ float* dst, uint32_t value, const asc_fill_value_config& config)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (matrix). |
| value | Input | Source operand (scalar). |
| config | Input | Initialization-related parameters used. For details, see [asc_fill_value_config](../defs/union/asc_fill_value_config.md). |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- If the data types of dst and value are different, the value of value must be converted according to the data type of dst. For details, see the calling example.

## Examples

```cpp
constexpr uint64_t total_length = 256;    // total_length indicates the total length of data involved in the transfer
asc_fill_value_config config;

// The data types of dst and value are different
__cb__ float dst0[total_length];
uint32_t value0 = 0x3F800000;  // The binary representation of float 1 is 0x3F800000.
config.repeat = 1;
config.blk_num = total_length * sizeof(float) / 512;
config.dst_gap = 0;
asc_fill_l0b_sync(dst0, value0, config);    // Initialize the 256 float elements in dst0 to 1.

// dst and value have the same data type.
__cb__ half dst1[total_length];
half value1 = 1;  // No binary conversion is needed; directly fill in 1.
config.repeat = 1;
config.blk_num = total_length * sizeof(half) / 512;
config.dst_gap = 0;
asc_fill_l0b_sync(dst1, value1, config);    // Initialize the 256 half elements in dst1 to 1.
```

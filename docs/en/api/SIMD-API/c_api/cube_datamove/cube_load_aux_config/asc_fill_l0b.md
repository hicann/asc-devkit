---
okftype: reference.api
description: "Writes the binary value of the scalar fill value into the cube area starting at dst in the L0B Buffer, repeatedly filling according to repeat, blk_num, and dst_gap of asc_fill_value_config, used for initializing the right cube input buffer."
tag:
  - roles: [data_initialization]
  - operations: [fill]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/overview/cube_compute_flow.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/L1_L0A_B_memory_structure_intro.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/defs/union/asc_fill_value_config.md
    kind: requires
    condition: "All overloads require passing config to specify the number of iterations, the number of data blocks per operation, and the block gap"
  - path: docs/zh/api/SIMD-API/c_api/general_description_and_constraints.md
    kind: constrained_by
    condition: "L0B storage unit capacity, alignment requirements, and general address overlap constraints"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_fill_l0b__PCBbf16_f16_RCfillcfg"
    scope_basis: resolved
    note: "Starting from 9.1.0, include and impl synchronously add the overload with bfloat16_t* dst and half fill value; 9.0.0 and earlier do not have this signature. The current @override_id of this overload in include is asc_fill_l0b__PCBbf16_f16_RCfillcfg, @archs 2201."
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_fill_l0b__PCBbf16_u32_RCfillcfg"
    scope_basis: resolved
    note: "Starting from 9.1.0, include and impl synchronously add the overload with bfloat16_t* dst and uint32_t fill value; 9.0.0 and earlier do not have this signature. The current @override_id of this overload in include is asc_fill_l0b__PCBbf16_u32_RCfillcfg, @archs 2201."
---
# asc_fill_l0b

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:24:53.619Z pushedAt=2026-11-18T08:15:34.317Z nl=2 -->

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

Assigns the binary value of value to dst to initialize the Local Memory of the L0B Buffer.

The fill operation is used to prepare the cube input buffer. For the subsequent data movement and cube multiply-accumulate flow, see [Cube Computation Flow](../../cube_compute/overview.md#cube-computation-flow); for the input buffer layout, see [L1 Buffer, L0A Buffer/L0B Buffer Memory Structure Introduction](../cube_compute_load/L1_L0A_B_memory_structure_intro.md).

## Prototype

- Regular computation

    ```cpp
    __aicore__ inline void asc_fill_l0b(__cb__ int16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ int16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ uint16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ half* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ half* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ bfloat16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ int32_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ int32_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ uint32_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ float* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ float* dst, uint32_t value, const asc_fill_value_config& config)
    ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (matrix). |
| value | Input | Source operand (scalar). |
| config | Input | Initialization-related parameters used. For details, see [asc_fill_value_config](../../defs/union/asc_fill_value_config.md). |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- For the space size and alignment requirements of each storage unit, see [Storage Unit Description](../../general_description_and_constraints.md#storage-unit-description).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../general_description_and_constraints.md#general-address-overlap-constraints).
- If the data types of dst and value are different, the value of value must be converted according to the data type of dst. For details, see the calling example.

## Examples

```cpp
constexpr uint64_t total_length = 256;    // total_length indicates the total length of data involved in the transfer.
asc_fill_value_config config;

// The data types of dst and value are different.
__cb__ float dst0[total_length];
uint32_t value0 = 0x3F800000;  // The binary representation of 1 in float type is 0x3F800000.
config.repeat = 1;
config.blk_num = total_length * sizeof(float) / 512;
config.dst_gap = 0;
asc_fill_l0b(dst0, value0, config);    // Initialize the 256 float-type elements in dst0 to 1.

// The data types of dst and value are the same.
__cb__ half dst1[total_length];
half value1 = 1;  // No binary conversion is needed; directly fill with 1.
config.repeat = 1;
config.blk_num = total_length * sizeof(half) / 512;
config.dst_gap = 0;
asc_fill_l0b(dst1, value1, config);    // Initialize the 256 half-type elements in dst1 to 1.
```

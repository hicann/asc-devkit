---
okftype: reference.api
description: "Reads the start address of the quantization parameters used for accompanying quantization when data is moved out of the L0C Buffer, and returns the quant_pre_addr configuration field written by asc_set_l0c_copy_config in the FPC register, rather than the data after quantization calculation."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: related
    condition: "The returned address index is configured by the quant_pre_addr parameter of this API"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md
    kind: related
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "In 9.1.0, the declaration is located in include/c_api/cube_compute/cube_compute.h. Starting from 9.2.0, the declaration of this symbol is moved to include/c_api/cube_datamove/cube_datamove.h and continues through 9.3.0-beta.1 (the signature remains unchanged, still taking no parameters and returning uint64_t). The header file path in the page function description is written according to the migrated location."
---
# asc_get_l0c2gm_prequant

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:46:10.274Z pushedAt=2026-11-18T08:15:34.346Z nl=2 -->

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

When accompanying quantization is performed during data movement, call this API to obtain the start address of the vector before the quantization operation.

For the quantization mode, parameter encoding, and preparation process, see [Accompanying Quantization](../cube_store_key_features/accompanying_quantization.md); for the coordination between quantization and activation parameters, see [Accompanying Quantization and ReLU Scenario Combination](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md).

This state is configured by [asc_set_l0c_copy_config](asc_set_l0c_copy_config.md). For the meaning and configuration override behavior of its `quant_pre_addr` parameter, see the corresponding API. The return value is a configuration field in the FPC register, not the data after quantization or activation computation.

## Prototype

```cpp
__aicore__ inline uint64_t asc_get_l0c2gm_prequant()
```

## Parameters

None

## Return Value

The start address of the vector before the quantization operation.

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

```cpp
uint64_t prequant_addr = asc_get_l0c2gm_prequant();
```

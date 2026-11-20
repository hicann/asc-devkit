---
okftype: reference.api
description: "Reads the UnitFlag global state setting used for L0C Buffer data movement, and returns the value configured by the is_clean_unit_flag parameter of asc_set_l0c_copy_config in the FPC register, which is used to confirm the UnitFlag coordination state between data movement and cube computation."
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_key_features/UnitFlag.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: related
    condition: "The return state is configured by the is_clean_unit_flag parameter of this API"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "In 9.1.0, the declaration is located in include/c_api/cube_compute/cube_compute.h. Starting from 9.2.0, it is moved to include/c_api/cube_datamove/cube_datamove.h and continues through 9.3.0-beta.1, with the signature remaining unchanged as a parameterless function returning uint64_t."
---
# asc_get_l0c2gm_unitflag

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:47:17.421Z pushedAt=2026-11-18T08:15:34.349Z nl=2 -->

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

When performing in-transit quantization during data movement, call this API to obtain the unit_flag setting.

This state is configured by [asc_set_l0c_copy_config](asc_set_l0c_copy_config.md). For the meaning of its `is_clean_unit_flag` parameter and the configuration override behavior, see the corresponding API. For how UnitFlag works with compute and move-out instructions, see [UnitFlag Feature Description](../../cube_compute/mmad_compute_key_features/UnitFlag.md).

## Prototype

```cpp
__aicore__ inline uint64_t asc_get_l0c2gm_unitflag()
```

## Parameters

None

## Return Value

unit_flag setting. unit_flag is a fine-grained parallelism between cube compute instructions and cube move instructions. When this feature is enabled, the hardware moves out the compute result as soon as each fractal is computed. This feature is not applicable to L0C Buffer accumulation scenarios.

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

```cpp
uint64_t unitflag_value = asc_get_l0c2gm_unitflag();
```

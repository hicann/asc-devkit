---
okftype: reference.api
description: "Deprecated interface page: asc_set_l0c2gm_lrelu_alpha sets the Leaky ReLU alpha value used by asc_copy_l0c2l1 and asc_copy_l0c2gm with a half or float reference. It is recommended to use asc_set_l0c_copy_lrelu_alpha, which provides only the float form."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: configures
    condition: "The alpha value is consumed by this copy-out interface during in-line Leaky ReLU"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md
    kind: configures
    condition: "The alpha value is consumed by this copy-out interface during in-line Leaky ReLU"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires switching to the new interface, which supports only float parameters"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The Release Notes 9.2.0 deprecation table lists the half& and float& overloads of asc_set_l0c2gm_lrelu_alpha as deprecated. The replacement interface asc_set_l0c_copy_lrelu_alpha supports only float. The prototype on this page is still retained, indicating deprecation rather than removal."
---
# asc_set_l0c2gm_lrelu_alpha (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:50:58.688Z pushedAt=2026-11-18T03:12:16.662Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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

**The `asc_set_l0c2gm_lrelu_alpha` API is deprecated. Use [asc_set_l0c_copy_lrelu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md) instead.**

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Sets the Leaky ReLU alpha value used during computation by the asc_copy_l0c2l1 or asc_copy_l0c2gm API. This value supports only the half and float data types.

## Prototype

```cpp
__aicore__ inline void asc_set_l0c2gm_lrelu_alpha(half& config)
__aicore__ inline void asc_set_l0c2gm_lrelu_alpha(float& config)
```

## Parameters

**Table 1** Parameter description

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| config | Input | Leaky ReLU alpha value. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

```cpp
half config = 0.5;
asc_set_l0c2gm_lrelu_alpha(config);
```

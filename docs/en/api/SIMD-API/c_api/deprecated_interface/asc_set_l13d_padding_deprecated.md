---
okftype: reference.api
description: "Deprecated interface page: asc_set_l13d_padding sets the Pad attribute descriptor in four forms (uint64_t, half, int16_t, or uint16_t) to configure the padding value for asc_copy_l12l0a. It is recommended to use asc_set_l12l0a_3d_padding instead."
tag:
  - roles: [configuration]
  - traits: [padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0a.md
    kind: configures
    condition: "This padding value is used when calling asc_copy_l12l0a for 3D data movement"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_l12l0a_3d_padding.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires using the renamed asc_set_l12l0a_3d_padding instead"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The Release Notes 9.2.0 deprecation table lists asc_set_l13d_padding as deprecated, replaced by the renamed interface asc_set_l12l0a_3d_padding; the four forms on this page are still retained, meaning this is a deprecation rather than a removal."
---
# asc_set_l13d_padding (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:55:42.398Z pushedAt=2026-11-18T03:12:16.668Z nl=2 -->

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

**The `asc_set_l13d_padding` API is deprecated. Use [asc_set_l12l0a_3d_padding](../cube_datamove/cube_load_aux_config/asc_set_l12l0a_3d_padding.md) instead.**

The header file path is `"c_api/cube_datamove/cube_datamove.h"`.

Sets the Pad attribute descriptor, which is used to configure the padding value when calling the [asc_copy_l12l0a](../cube_datamove/cube_compute_load/asc_copy_l12l0a.md) API.

## Prototype

```cpp
__aicore__ inline void asc_set_l13d_padding(uint64_t config)
__aicore__ inline void asc_set_l13d_padding(half config)
__aicore__ inline void asc_set_l13d_padding(int16_t config)
__aicore__ inline void asc_set_l13d_padding(uint16_t config)
```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| config | Input     | The value of the Pad padding value.|

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

```cpp
constexpr uint64_t config = 0;
asc_set_l13d_padding(config);
```

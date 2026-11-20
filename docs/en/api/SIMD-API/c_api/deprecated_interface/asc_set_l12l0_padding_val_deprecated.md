---
okftype: reference.api
description: "Deprecated API page: under the 3510 architecture, asc_set_l12l0_padding_val sets the PADDING_B register with a 64-bit config. Bits 31:0 store the padding value and use different bytes according to the data bit width. Bits 33:32 select fixed padding or per-channel padding. It is recommended to use asc_set_l12l0b_3d_padding instead."
tag:
  - roles: [configuration]
  - traits: [padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_load/asc_copy_l12l0b_3d_arch_3510.md
    kind: configures
    condition: "During 3D data movement, this API consumes the padding value and mode"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_load_aux_config/asc_set_l12l0b_3d_padding.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires using asc_set_l12l0b_3d_padding, whose function name has been adjusted"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The Release Notes 9.2.0 deprecation table lists asc_set_l12l0_padding_val as deprecated, replaced by asc_set_l12l0b_3d_padding, whose function name has been adjusted; the API remains available."
---
# asc_set_l12l0_padding_val (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:54:35.632Z pushedAt=2026-11-18T03:12:16.667Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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

**The `asc_set_l12l0_padding_val` API is deprecated. Use [asc_set_l12l0b_3d_padding](../cube_datamove/cube_load_aux_config/asc_set_l12l0b_3d_padding.md) instead.**

The header file path is `"c_api/cube_datamove/cube_datamove.h"`.

Sets the value in the PADDING_B register, which is a 64-bit register used to store the padding value during data movement.
The meaning of each bit is as follows:

**Table 1** PADDING_B register bit description

| Bit range |                                                                           Meaning                                                                           |
| :-----------|:------------------------------------------------------------------------------------------------------------------------------------------------------:|
| 31:0 | Stores the value used for padding.<br>When the data bit width is 32, bits 31:0 are used directly;<br> when the data bit width is 16, bits 15:0 are used and bits 31:16 are ignored; <br> when the data bit width is 8, bits 15:8 should be the same as bits 7:0, and bits 31:16 are ignored; when the data bit width is 4, bits 15:12, 11:8, 7:4, and 3:0 should be the same, and bits 31:16 are ignored. |
| 33:32 | Indicates the padding mode used. Three modes are currently supported.<br>2'b00: fixed padding;<br>2'b01: per-channel padding;<br>2'b10/2'b11: currently reserved. |

## Prototype

```cpp
__aicore__ inline void asc_set_l12l0_padding_val(uint64_t config)
```

## Parameters

**Table 2** Parameter description

|Parameter|Input/Output| Description        |
| :------ | :---  |:----------|
|config   |Input   | Register value to be set. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Examples

```cpp
uint64_t config = 0;
asc_set_l12l0_padding_val(config);
```

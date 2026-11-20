---
okftype: reference.api
description: "Deprecated interface page: On the 3510 architecture, asc_set_l0c2gm_relu_alpha uses a 64-bit config to set the M2 value of Scalar ReLU in the RELU_ALPHA register, where bits 31:13 and 63:45 correspond to ReLU_PRE and ReLU_POST respectively. It is recommended to use asc_set_l0c_copy_relu_alpha instead."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: configures
    condition: "The alpha value is consumed by this store-out interface during inline Scalar ReLU"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires replacing the uint64_t config with asc_set_l0c_copy_relu_alpha, which uses a float scalar parameter"
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The Release Notes 9.2.0 deprecation table lists asc_set_l0c2gm_relu_alpha as deprecated, and the replacement interface changes the uint64_t config to a float scalar. The prototype on this page is still retained, meaning it is deprecated rather than removed."
---
# asc_set_l0c2gm_relu_alpha (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:53:55.638Z pushedAt=2026-11-18T03:12:16.666Z nl=2 -->

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

**The `asc_set_l0c2gm_relu_alpha` interface is deprecated. Use the [asc_set_l0c_copy_relu_alpha](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md) interface instead.**

The header file path is `"c_api/cube_datamove/cube_datamove.h"`.

Sets the value in the RELU_ALPHA register. This is a 64-bit register that stores the alpha value used when performing Scalar ReLU in fixpipe or cube instructions.

The meaning of each bit is as follows:

**Table 1** RELU_ALPHA register bit description

| Bit range |                                           Description                                           |
|:------|:--------------------------------------------------------------------------------------:|
| 31:13 | Indicates the M2 value of Scalar ReLU in ReLU_PRE, which takes effect only for Scalar ReLU. The hardware uses it for computation in the format of 1-bit sign, 8-bit exponent, and 10-bit mantissa. It cannot be INF/NAN.  |
| 63:45 | Indicates the M2 value of Scalar ReLU in ReLU_POST, which takes effect only for Scalar ReLU. The hardware uses it for computation in the format of 1-bit sign, 8-bit exponent, and 10-bit mantissa. It cannot be INF/NAN. |

## Prototype

```cpp
__aicore__ inline void asc_set_l0c2gm_relu_alpha(uint64_t config)
```

## Parameters

**Table 2** Parameter description

| Parameter | Input/Output | Description        |
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
uint32_t pre_m2 = (0<<18)|(127<<10)|0;
uint32_t post_m2 = (0<<18)|(124<<10)|0;
uint64_t config = ((uint64_t)pre_m2<<13)|((uint64_t)post_m2<<45);
asc_set_l0c2gm_relu_alpha(config);
```

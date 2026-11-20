---
okftype: reference.api
description: "Deprecated interface page: asc_set_l0c2gm_config sets the parameters for in-line quantization in the vectorization process using the three parameters relu_pre, quant_pre start address, and enable_unit_flag. Quantization parameters cannot be inf or NaN. It is recommended to use asc_set_l0c_copy_config instead."
tag:
  - roles: [configuration]
  - traits: [quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md
    kind: configures
    condition: "This configuration is consumed by the copy-out interface during in-line quantization"
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md
    kind: replaced_by
    condition: "Release Notes 9.2.0 requires using the new interface with adjusted function name and parameter name"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The Release Notes 9.2.0 deprecation table lists asc_set_l0c2gm_config as deprecated, replaced by asc_set_l0c_copy_config; the interface remains available."
---
# asc_set_l0c2gm_config (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:49:26.057Z pushedAt=2026-11-18T03:12:16.661Z nl=2 -->

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

**The `asc_set_l0c2gm_config` interface is deprecated. Use the [asc_set_l0c_copy_config](../cube_datamove/cube_store_aux_config/asc_set_l0c_copy_config.md) interface instead.**

The header file path is `"c_api/cube_datamove/cube_datamove.h"`.

When performing in-transit quantization during data movement, call this interface to set the vector quantization parameters in the quantization process.

## Prototype

```cpp
__aicore__ inline void asc_set_l0c2gm_config(uint64_t relu_pre, uint64_t quant_pre, bool enable_unit_flag)
```

## Parameters

**Table 1** Parameter description

|Parameter|Input/Output|Description|
|------------|------------|-----------|
| relu_pre | Input     | Start address of the vector before the ReLU operation.|
| quant_pre | Input     | Start address of the vector before the quantization operation.|
| enable_unit_flag | Input     | Whether to enable unit_flag. unit_flag is a fine-grained parallelism between matrix computation instructions and matrix movement instructions. After this function is enabled, the computation result is moved out as soon as the hardware finishes computing each fractal. This function is not applicable to L0C Buffer accumulation scenarios.|

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

Quantization parameters cannot be inf/nan or denormalized numbers.

The following three parameter passing forms are supported:
- Set relu_pre and quant_pre at the same time.
- Pass only relu_pre, and pass 0 for quant_pre.
- Only quant_pre is passed in, and relu_pre is passed in as 0.

## Examples

```cpp
constexpr uint64_t relu_pre = 0;
constexpr uint64_t quant_pre = 0x1000;// Assume the effective address of the quantization operation is 0x1000
asc_set_l0c2gm_config(relu_pre, quant_pre, true);
```

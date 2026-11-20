---
okftype: reference.api
description: "Deprecated API page: On the 3510 architecture, asc_set_l0c2gm_quant_post sets the QUANT_POST register of the Fixpipe post-processing stage via a 64-bit config. Each bit of the register expresses the offset and M3 value of post-quantization modes such as S4, S8, and S16. No replacement API is available."
tag:
  - roles: [configuration]
  - traits: [quantization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md
    kind: related
    condition: "The meaning of each quantization mode in the register is defined in the quantization mode description of this copy-out API."
version: ">=9.1.0"
version_changes:
  - since: "9.2.0"
    kind: deprecated
    scope: "page"
    scope_basis: kind_default
    note: "The Release Notes 9.2.0 deprecation table lists asc_set_l0c2gm_quant_post as deprecated and notes that no replacement API is available; the API itself remains in the header file and has not been removed."
---
# asc_set_l0c2gm_quant_post (Deprecated)

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T14:53:17.122Z pushedAt=2026-11-18T03:12:16.665Z nl=2 -->

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

**The `asc_set_l0c2gm_quant_post` API is deprecated, and no replacement API is available.**

The header file path is: `"c_api/cube_datamove/cube_datamove.h"`.

Sets the value in the QUANT_POST register. QUANT_POST is a 64-bit register that stores the quantization information of the Fixpipe post-processing stage.
The meaning of each bit is as follows. For descriptions of the various quantization modes, see the descriptions of the quantization modes in [asc_copy_l0c2l1](../cube_datamove/cube_compute_store/asc_copy_l0c2l1_arch_3510.md):

**Table 1** QUANT_POST register bit description

| Bit range |                                                           Meaning                                                           |
| :-----------|:----------------------------------------------------------------------------------------------------------------------:|
| 4:0 |                                In the QS162S4_POST, QF162S4_POST, and SHIFT2S4_POST quantization modes, indicates the offset in s5.                                |
| 8:0 | In the QS162B8_POST, QF162B8_POST, and SHIFT2B8_POST quantization modes, indicates the offset in s9, or in the QS162S16_POST, QF162S16_POST, and SHIFT2S16_POST quantization modes, indicates the lower 9 bits of the s17 offset. |
| 9 |                             In the QS162B8_POST, QF162B8_POST, and SHIFT2B8_POST quantization modes, indicates whether the quantization result is signed.                              |
| 31:13 |         In the QS162B8_POST, QF162B8_POST, QS162S4_POST, QF162S4_POST, QS162S16_POST, and QF162S16_POST quantization modes, indicates M3. It cannot be INF/NAN.          |
| 39:32 |                           In the QS162S16_POST, QF162S16_POST, SHIFT2S16_POST, and SHIFT2S32_POST quantization modes, indicates the upper 8 bits of the s17 offset. |
| 44:40 | In the SHIFT2S4_POST, SHIFT2B8_POST, SHIFT2S16_POST, and SHIFT2S32_POST quantization modes, indicates the shift value. |

## Prototype

```cpp
__aicore__ inline void asc_set_l0c2gm_quant_post(uint64_t config)
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

The quantization parameters cannot be inf/nan or denormalized numbers.

## Examples

```cpp
const uint64_t MASK_8_0 = 0x1FF;
const uint64_t MASK_BIT9 = 0x200;
const uint64_t MASK_31_13 = 0xFFFFE000;

uint64_t field_8_0 = 0x1AB;
uint64_t bit_9 = 1;
uint64_t field_31_13 = 0x000EF;

uint64_t config = 0;

config = (config & ~MASK_8_0) | (field_8_0 & MASK_8_0);
config = (config & ~MASK_BIT9) | ((bit_9<<9) & MASK_BIT9);
config = (config & ~MASK_31_13) | (field_31_13 & MASK_31_13);
asc_set_l0c2gm_quant_post(config);
```

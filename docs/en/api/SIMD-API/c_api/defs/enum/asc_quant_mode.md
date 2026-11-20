---
okftype: reference.enum
description: "Type alias for the type conversion or quantization mode when L0C Buffer data is moved out, equivalent to QuantMode_t provided by the compiler. It serves as the type of the quant_pre_mode parameter of the cube move-out interface. The available values vary by product model and specific interface."
tag:
  - roles: [data_movement_configuration]
  - operations: [conversion]
  - traits: [quantization]
dependencies: []
version: ">=9.2.0"
version_changes: []
---
# asc_quant_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:12:47.132Z pushedAt=2026-11-18T08:15:34.398Z nl=2 -->

`asc_quant_mode` is used to represent the type conversion or quantization mode when L0C Buffer data is moved out, serving as the type of the `quant_pre_mode` parameter in the cube move-out interface.

The header file path is: `"c_api/defs/enum.h"`.

## Type Alias Definition

```cpp
using asc_quant_mode = QuantMode_t;
```

`asc_quant_mode` has the same enum values as `QuantMode_t`. `QuantMode_t` is provided by the compiler environment, and its enumerators do not explicitly specify underlying integer values.

## Enum Value Details

Different product models support different quantization modes, and the range of enum values supported by a specific interface may also differ. When using them, refer to the description of the `quant_pre_mode` parameter in the corresponding interface. The current product support is as follows:

<!-- npu="910b" id1 -->
- For Atlas A2 products, the supported enum values are: `NoQuant`, `F322F16`, `F322BF16`, `DEQF16`, `VDEQF16`, `QF322B8_PRE`, `VQF322B8_PRE`, `REQ8`, `VREQ8`. Some cube move-out interfaces also support `REQ4`, `VREQ4`, `QF322S4_PRE`, and `VQF322S4_PRE`.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- For Atlas A3 products, the supported enum values are: `NoQuant`, `F322F16`, `F322BF16`, `DEQF16`, `VDEQF16`, `QF322B8_PRE`, `VQF322B8_PRE`, `REQ8`, `VREQ8`. Some cube move-out interfaces also support `REQ4`, `VREQ4`, `QF322S4_PRE`, and `VQF322S4_PRE`.
<!-- end id2 -->
<!-- npu="950" id3 -->
- For Ascend 950PR&950DT products, all enum values listed in the following table are supported.

    **Table 1** Enum value description

    | Enum Value | Description |
    | --- | --- |
    | NoQuant | Quantization is not enabled. |
    | F322F16 | Converts `float` to `half`. |
    | F322BF16 | Converts `float` to `bfloat16_t`. |
    | DEQF16 | Quantizes `int32_t` to `half` using Scalar quantization parameters. |
    | VDEQF16 | Quantizes `int32_t` to `half` using Tensor quantization parameters. |
    | REQ4 | Quantizes `int32_t` to `int4b_t` using Scalar quantization parameters. |
    | VREQ4 | Quantizes `int32_t` to `int4b_t` using Tensor quantization parameters. |
    | QF322B8_PRE | Quantizes `float` to `int8_t` or `uint8_t` using Scalar quantization parameters. |
    | VQF322B8_PRE | Quantizes `float` to `int8_t` or `uint8_t` using Tensor quantization parameters. |
    | REQ8 | Quantizes `int32_t` to `int8_t` or `uint8_t` using Scalar quantization parameters. |
    | VREQ8 | Quantizes `int32_t` to `int8_t` or `uint8_t` using Tensor quantization parameters. |
    | QF322S4_PRE | Quantizes `float` to `int4b_t` using Scalar quantization parameters. |
    | VQF322S4_PRE | Quantizes `float` to `int4b_t` using Tensor quantization parameters. |
    | QF322FP8_PRE | Quantizes `float` to `fp8_e4m3fn_t` using Scalar quantization parameters. |
    | VQF322FP8_PRE | Quantizes `float` to `fp8_e4m3fn_t` using Tensor quantization parameters. |
    | QF322HIF8_PRE | Quantizes `float` to `hifloat8_t` using Scalar quantization parameters, with Half to Away rounding. |
    | VQF322HIF8_PRE | Quantizes `float` to `hifloat8_t` using Tensor quantization parameters, with Half to Away rounding. |
    | QF322HIF8_PRE_HYBRID | Quantizes `float` to `hifloat8_t` using Scalar quantization parameters, with Hybrid rounding. |
    | VQF322HIF8_PRE_HYBRID | Quantizes `float` to `hifloat8_t` using Tensor quantization parameters, with Hybrid rounding. |
    | QS322BF16_PRE | Quantizes `int32_t` to `bfloat16_t` using Scalar quantization parameters. |
    | VQS322BF16_PRE | Quantizes `int32_t` to `bfloat16_t` using Tensor quantization parameters. |
    | QF322F16_PRE | Quantizes `float` to `half` using Scalar quantization parameters. |
    | VQF322F16_PRE | Quantizes `float` to `half` using Tensor quantization parameters. |
    | QF322BF16_PRE | Quantizes `float` to `bfloat16_t` using Scalar quantization parameters. |
    | VQF322BF16_PRE | Quantizes `float` to `bfloat16_t` using Tensor quantization parameters. |
    | QF322F32_PRE | Quantizes `float` to `float` using Scalar quantization parameters. |
    | VQF322F32_PRE | Quantizes `float` to `float` using Tensor quantization parameters. |
<!-- end id3 -->
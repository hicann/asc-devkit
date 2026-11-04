# asc_quant_mode

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T00:04:17.751Z pushedAt=2026-10-28T12:47:03.104Z nl=2 -->

`asc_quant_mode` indicates the type conversion or quantization mode used when moving L0C Buffer data out, and serves as the type of the `quant_pre_mode` parameter in the matrix move-out interface.

The header file path is `"c_api/defs/enum.h"`.

## Type Alias Specific Definition

```cpp
using asc_quant_mode = QuantMode_t;
```

`asc_quant_mode` and `QuantMode_t` have the same enum values. `QuantMode_t` is provided by the compiler environment, and its enum items do not explicitly specify underlying integer values.

## Enum Value Details

Different product models support different quantization modes, and the range of enum values supported by a specific API may also differ. When using an API, refer to the description of the `quant_pre_mode` parameter in that API. The current product support is as follows:

<!-- npu="910b" id1 -->
- For Atlas A2 products, the supported enum values are `NoQuant`, `F322F16`, `F322BF16`, `DEQF16`, `VDEQF16`, `QF322B8_PRE`, `VQF322B8_PRE`, `REQ8`, and `VREQ8`. Some matrix move-out APIs also support `REQ4`, `VREQ4`, `QF322S4_PRE`, and `VQF322S4_PRE`.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- For Atlas A3 products, the supported enum values are `NoQuant`, `F322F16`, `F322BF16`, `DEQF16`, `VDEQF16`, `QF322B8_PRE`, `VQF322B8_PRE`, `REQ8`, and `VREQ8`. Some matrix move-out APIs also support `REQ4`, `VREQ4`, `QF322S4_PRE`, and `VQF322S4_PRE`.
<!-- end id2 -->
<!-- npu="950" id3 -->
- For Ascend 950PR&950DT products, all enum values listed in the following table are supported.

    **Table 1** Enum value description

    | Enum Value | Description |
    | --- | --- |
    | NoQuant | Disables quantization. |
    | F322F16 | Converts `float` to `half`. |
    | F322BF16 | Converts `float` to `bfloat16_t`. |
    | DEQF16 | Quantizes `int32_t` to `half` using the Scalar quantization parameter. |
    | VDEQF16 | Quantizes `int32_t` to `half` using the Tensor quantization parameter. |
    | REQ4 | Quantizes `int32_t` to `int4b_t` using the Scalar quantization parameter. |
    | VREQ4 | Quantizes `int32_t` to `int4b_t` using the Tensor quantization parameter. |
    | QF322B8_PRE | Quantizes `float` to `int8_t` or `uint8_t` using the Scalar quantization parameter. |
    | VQF322B8_PRE | Quantizes `float` to `int8_t` or `uint8_t` using the Tensor quantization parameter. |
    | REQ8 | Quantizes `int32_t` to `int8_t` or `uint8_t` using the Scalar quantization parameter. |
    | VREQ8 | Quantizes `int32_t` to `int8_t` or `uint8_t` using the Tensor quantization parameter. |
    | QF322S4_PRE | Quantizes `float` to `int4b_t` using the Scalar quantization parameter. |
    | VQF322S4_PRE | Quantizes `float` to `int4b_t` using the Tensor quantization parameter. |
    | QF322FP8_PRE | Quantizes `float` to `fp8_e4m3fn_t` using the Scalar quantization parameter. |
    | VQF322FP8_PRE | Quantizes `float` to `fp8_e4m3fn_t` using the Tensor quantization parameter. |
    | QF322HIF8_PRE | Quantizes `float` to `hifloat8_t` using the Scalar quantization parameter, with Half to Away rounding. |
    | VQF322HIF8_PRE | Quantizes `float` to `hifloat8_t` using the Tensor quantization parameter, with Half to Away rounding. |
    | QF322HIF8_PRE_HYBRID | Quantizes `float` to `hifloat8_t` using the Scalar quantization parameter, with Hybrid rounding. |
    | VQF322HIF8_PRE_HYBRID | Quantizes `float` to `hifloat8_t` using the Tensor quantization parameter, with Hybrid rounding. |
    | QS322BF16_PRE | Quantizes `int32_t` to `bfloat16_t` using the Scalar quantization parameter. |
    | VQS322BF16_PRE | Quantizes `int32_t` to `bfloat16_t` using the Tensor quantization parameter. |
    | QF322F16_PRE | Quantizes `float` to `half` using the Scalar quantization parameter. |
    | VQF322F16_PRE | Quantizes `float` to `half` using the Tensor quantization parameter. |
    | QF322BF16_PRE | Quantizes `float` to `bfloat16_t` using the Scalar quantization parameter. |
    | VQF322BF16_PRE | Quantizes `float` to `bfloat16_t` using the Tensor quantization parameter. |
    | QF322F32_PRE | Quantizes `float` to `float` using the Scalar quantization parameter. |
    | VQF322F32_PRE | Quantizes `float` to `float` using the Tensor quantization parameter. |
<!-- end id3 -->

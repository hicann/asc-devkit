# GetSaturationFlag(ISASI)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:28:37.990Z pushedAt=2026-10-26T07:41:44.774Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/GetSaturationFlag_ISASI_res.md#id1 -->

## Function Description

The header file path is `"basic_api/kernel_operator_common_intf.h"`.

Obtains the enabled state of the global Saturation Mode specified by `mode`, which is set through [SetSaturationFlag](SetSaturationFlag_ISASI.md). Saturation Mode is used to handle scenarios where results overflow or INF/NAN occur during computation or precision conversion. For details, see [SetSaturationFlag Function Description](SetSaturationFlag_ISASI.md#description).

## Prototype

```cpp
template <SaturationMode mode>
__aicore__ inline bool GetSaturationFlag()
```

## Parameters

**Table 1**  Template parameter description

| Parameter Name | Description |
| --- | --- |
| mode | Saturation Mode, with the following values:<br>&bull;`SaturationMode::FLOAT`: Saturation mode for floating-point computation and floating-point precision conversion, enabled by default.<br>&bull;`SaturationMode::FLOAT8`: During floating-point precision conversion, controls whether FP8 output is saturated to 0 when it is NaN, enabled by default. This mode takes effect only when the precision conversion API is in saturation mode. For details, see [Constraint Description](#constraint-description).<br>&bull;`SaturationMode::INT`: Saturation mode for integer computation, disabled by default.<br>&bull;`SaturationMode::CAST`: Saturation mode for precision conversion when converting floating-point numbers to integers or integers to integers, enabled by default.<br>For the data types affected by each mode, see [SetSaturationFlag data types](SetSaturationFlag_ISASI.md#data-type). The template parameter has no default value and must be explicitly specified when calling this API. |

## Return Value

Returns `true` if the global saturation mode specified by `mode` is enabled, and `false` if the mode is disabled.

## Constraint Description

<!-- npu="950" id10 -->
- The Ascend 950PR&950DT products supports `SaturationMode::FLOAT`, `SaturationMode::FLOAT8`, `SaturationMode::INT`, and `SaturationMode::CAST`.
- When querying `SaturationMode::FLOAT8`, the return value only indicates whether this mode is enabled. This mode takes effect only when the precision conversion API is in Saturation Mode, which can be entered through the global `SaturationMode::FLOAT` or the per-API `satMode`.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- The Atlas A3 products supports `SaturationMode::FLOAT`, `SaturationMode::INT`, and `SaturationMode::CAST`, but does not support `SaturationMode::FLOAT8`.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- The Atlas A2 products supports `SaturationMode::FLOAT`, `SaturationMode::INT`, and `SaturationMode::CAST`, but does not support `SaturationMode::FLOAT8`.
<!-- end id12 -->

## Example

The following example queries whether the saturation mode for integer computation is enabled.

```cpp
bool enableSat = AscendC::GetSaturationFlag<AscendC::SaturationMode::INT>();
```

# Rounding Mode and Saturation Mode

<!-- md-trans-meta sourceCommit=63c0b0eaa8a2923ff3c351fb2a0592ad269f94d6 translatedAt=2026-11-19T03:00:56.658Z pushedAt=2026-11-19T05:54:06.680Z nl=2 -->

## Overview

The Tensor API completes register data type conversion through the `cast` API. For conversions that may incur precision loss or value overflow, the rounding mode and saturation mode can be specified through the compile-time `cast_options`:

- **Rounding mode**: Specifies the rounding direction or rule when precision loss occurs.
- **Saturation mode**: Specifies the handling method when the conversion result exceeds the representable range of the destination type.

The related API declarations are located in `tensor_api/experimental/arch/vector/type_conversion.h`, and the related enumeration definitions are located in `tensor_api/experimental/utils/reg_enum.h`.

## Rounding Mode

`cast_round_mode` is used to specify the rounding method of `cast`. The Tensor API uses enum values to configure the mode.

| Enum Value | Description | Example |
| --- | --- | --- |
| rint | Rounds to the nearest value; rounds to even when exactly at the intermediate value. | 1.5 → 2, 2.5 → 2 |
| round | Rounds to the nearest value; rounds away from 0 when exactly at the intermediate value. | 1.5 → 2, -1.5 → -2 |
| floor | Rounds toward negative infinity. | 1.7 → 1, -1.3 → -2 |
| ceil | Rounds toward positive infinity. | 1.3 → 2, -1.7 → -1 |
| trunc | Rounds toward 0, directly truncating the fractional part. | 1.7 → 1, -1.7 → -1 |
| odd | Nearest-neighbor odd rounding, handled according to the odd rounding rule of the destination floating-point format. Currently only `float → half` supports this mode. | - |
| hybrid | Mixed rounding mode, currently used for converting `float` or `half` to `hifloat8_t`. | - |
| unknown | Reserved value for an unspecified rounding method. Used only when the corresponding type conversion supports this configuration. | - |

### Configuration Rules

- The examples of `rint`, `round`, `floor`, `ceil`, and `trunc` in the table use floating-point to integer conversion as an example. For floating-point to floating-point conversion, the rounding target is a number representable by the destination format, rather than an integer.
- The rounding mode is part of the `cast` template parameter `opt` and is determined at compile time.
- Different source types and destination types support different rounding modes, which must be configured according to the [cast data type](cast.md#data-type) table.
- Unsupported combinations of type, layout, rounding, and saturation configurations trigger a compile-time error.
- For exact conversions that do not require selecting a rounding method, `unknown` must be used; the specific value is determined by the [cast data type](cast.md#data-type) table.

### Rounding Process Examples

The following uses the in-range `float → int16_t` conversion as an example to illustrate how common rounding modes handle positive numbers, negative numbers, and intermediate values:

| Input | `rint` | `round` | `floor` | `ceil` | `trunc` |
| --- | --- | --- | --- | --- | --- |
| 1.4 | 1 | 1 | 1 | 2 | 1 |
| 1.5 | 2 | 2 | 1 | 2 | 1 |
| 2.5 | 2 | 3 | 2 | 3 | 2 |
| -1.4 | -1 | -1 | -2 | -1 | -1 |
| -1.5 | -2 | -2 | -2 | -1 | -1 |
| -2.5 | -2 | -3 | -3 | -2 | -2 |

Among them, both `rint` and `round` select the nearest value, but they handle intermediate values differently: `2.5` yields the even number `2` in `rint` mode and `3` away from 0 in `round` mode. `floor` and `trunc` also handle negative numbers differently: `-1.4` yields `-2` and `-1`, respectively.

`odd` is used for odd rounding of the destination floating-point format and cannot be applied to the integer rounding table above. For example, `1.5` can be exactly represented by `half`, so it cannot be interpreted as the integer `1` or `3` when converted to `half`. `hybrid` is used for mixed rounding in `hifloat8_t`-related conversions and should not be understood as a fixed round-up, round-down, or round-toward-zero rule.

## Saturation Mode and Non-Saturation Mode

`cast_sat_mode` is used to specify the handling method when the conversion result exceeds the representable range of the destination type.

| Enum Value | Description |
| --- | --- |
| not_sat | Non-saturation mode. Truncates to the destination format when an integer narrowing conversion is out of range; floating-point conversion preserves the special values supported by the destination floating-point format. |
| sat | Saturation mode. Clamps to the destination type boundary when the conversion result exceeds the destination type range. |
| unknown | Reserved value for not configuring the saturation mode, used only in scenarios where the corresponding type conversion supports this configuration. |

### Configuration Method

When enabling saturation mode and non-saturation mode, the CTRL register must be configured. Save the current value through the [asc_get_ctrl API](../../../c_api/spr/asc_get_ctrl.md), modify only the required bits, and restore the original value after the related calculation completes.

| CTRL Bit | Description | Configuration Value |
| --- | --- | --- |
| CTRL[60] | Selects single-instruction/API configuration or global saturation configuration. | 0: Use single-instruction/API configuration; 1: Use global configuration. |
| CTRL[59] | Controls the global conversion saturation mode for floating-point to integer and integer to integer conversions, effective only when `CTRL[60] = 1`. | 0: Saturation; 1: Truncation. |
| CTRL[48] | Controls the global saturation behavior of floating-point calculations and some floating-point precision conversions. | 0: Saturation, INF clamped to the corresponding finite boundary and NaN converted to 0; 1: Non-saturation, INF and NaN preserved. |
| CTRL[50] | Controls the global saturation mode for scenarios related to `fp8_e8m0_t`, `fp8_e5m2_t`, and `fp8_e4m3fn_t`. | 0: Saturation; 1: Non-saturation. |

#### Configuration by cast_options

To select the saturation behavior of each conversion separately through `cast_options::sat`, set `CTRL[60]` to `0`, and then call `cast` with `cast_sat_mode::sat` or `cast_sat_mode::not_sat`. The CTRL configuration is completed outside the SIMD VF and before executing the related conversions; it is restored after the related calculation completes:

```cpp
#include "c_api/spr/spr.h"

const uint64_t saved_ctrl = static_cast<uint64_t>(asc_get_ctrl());
const uint64_t api_ctrl = saved_ctrl & ~(uint64_t{1} << 60);
asc_set_ctrl(api_ctrl);
```

After executing the register conversion shown in the [Configuration Example](#configuration-example) on this page, restore outside the SIMD VF:

```cpp
asc_set_ctrl(saved_ctrl);
```

#### Configuration by Global CTRL

For floating-point to integer or integer to integer conversions, if the global mode is used, set `CTRL[60]` to `1` and specify saturation or truncation through `CTRL[59]`. The following is a configuration fragment for the global saturation mode:

```cpp
const uint64_t saved_ctrl = static_cast<uint64_t>(asc_get_ctrl());
const uint64_t global_sat_ctrl = (saved_ctrl | (uint64_t{1} << 60)) & ~(uint64_t{1} << 59);
asc_set_ctrl(global_sat_ctrl);
```

To switch to the global truncation mode, use `asc_set_ctrl(global_sat_ctrl | (uint64_t{1} << 59))`. In global mode, the final saturation behavior cannot be determined solely from `cast_options::sat`, and the `cast_options` supported by the type combination must still be passed. After the conversion completes, restore with `asc_set_ctrl(saved_ctrl)` as well.

For floating-point to floating-point scenarios, configure `CTRL[48]` and the applicable `CTRL[50]` according to the destination type; `CTRL[59]` cannot be used as a substitute. `FLOAT` global saturation requires the global strategy to be enabled first; `FLOAT8` saturation also requires `FLOAT` saturation to be enabled first. For the applicable scope and configuration constraints of each mode, see [asc_set_ctrl](../../../c_api/spr/asc_set_ctrl.md) and [asc_set_saturation_flag](../../../c_api/spr/asc_set_saturation_flag.md). Modifying CTRL affects other calculations on the current core that are controlled by these bits, so a fixed value cannot be written directly and overwrite unrelated configurations.

### Mode Comparison

The behavior in different conversion scenarios is as follows; whether a specific type conversion supports `not_sat` or `sat` is determined by the [cast data type](cast.md#data-type) table.

| Conversion Scenario | `not_sat` | `sat` |
| --- | --- | --- |
| Floating-point to integer | Truncates to the destination format when out of range; positive and negative infinity return the corresponding boundary values of the destination type; NaN returns 0. | Returns the destination type boundary value when out of range; positive and negative infinity are clamped to the corresponding boundaries respectively; NaN returns 0. |
| Floating-point to floating-point | Preserves NaN and positive/negative infinity when the destination type supports them; finite values out of range are converted according to the destination floating-point format. | Clamps to the destination floating-point type boundary when out of range; NaN is converted to 0. |
| Integer to floating-point | This scenario does not provide a saturation mode; the rounding mode is used to control possible precision loss. | Not supported. |
| Integer to integer | Truncates to the data width of the destination format when a narrowing conversion is out of range; widening conversion usually preserves the value. | Clamps to the destination type boundary when out of range. |

For example, when `int32_t` is converted to `uint8_t`, the input `256` yields `0` in `not_sat` mode and `255` in `sat` mode; the input `-1` yields `0` in `sat` mode.

### Integer to Integer Example

Taking `int32_t → uint8_t` as an example, the non-saturation mode retains the low 8 bits rather than clamping the value to within range:

| Input | Non-Saturation Result | Saturation Result |
| --- | --- | --- |
| 100 | 100 | 100 |
| 256 (0x00000100) | 0 (0x00) | 255 |
| 511 (0x000001FF) | 255 (0xFF) | 255 |
| -1 (0xFFFFFFFF) | 255 (0xFF) | 0 |

Widening conversion usually preserves the value, but this cannot be used to infer any signed-to-unsigned conversion. For example, `int16_t → uint32_t` requires `opt.sat` to be `unknown`, and negative numbers are converted to `0`. Refer to the [cast data type description](cast.md#data-type) and the corresponding conversion constraints.

### Floating-Point to Integer Example

Taking `float → int16_t` as an example, the fractional part is first handled according to the selected rounding mode, and then the results that exceed the destination type range are handled according to the effective saturation configuration. The following table uses `rint`:

| Input | Non-Saturation Result | Saturation Result |
| --- | --- | --- |
| 2.5 | 2 | 2 |
| 40000.0 | -25536, retaining the low 16 bits | 32767 |
| -40000.0 | 25536, retaining the low 16 bits | -32768 |
| Positive infinity | 32767 | 32767 |
| Negative infinity | -32768 | -32768 |
| NaN | 0 | 0 |

### Floating-Point to Floating-Point Example

Taking `float → half` as an example, the maximum finite positive number of the destination type is `65504.0`. The following table uses `rint` and assumes that CTRL has been configured according to the corresponding mode:

| Input | Non-Saturation Result | Saturation Result |
| --- | --- | --- |
| 1.5 | 1.5 | 1.5 |
| 70000.0 | Positive infinity | 65504.0 |
| -70000.0 | Negative infinity | -65504.0 |
| Positive infinity | Positive infinity | 65504.0 |
| Negative infinity | Negative infinity | -65504.0 |
| NaN | NaN | 0 |

The above results cannot be generalized to all low-precision floating-point formats or all rounding modes. The handling of NaN, infinity, and finite overflow values by other destination formats is determined by the constraints of the corresponding type conversion.

## Configuration Example

The following fragment converts `float` to `half` within a SIMD VF or SIMD callee function, using round-to-nearest-even and saturation mode. `src` is a `reg_tensor<float>` with data and mask already initialized; before the call, the CTRL saturation strategy must be set according to [Configuration Method](#configuration-method):

```cpp
static constexpr asc::te::experimental::cast_options options = {
    asc::te::experimental::cast_layout::zero,
    asc::te::experimental::cast_round_mode::rint,
    asc::te::experimental::cast_sat_mode::sat,
};

auto dst = asc::te::experimental::cast<half, options>(src);
```

`cast_layout` is used to describe the position of the smaller element in the register when the source and destination types have different bit widths; it is not part of the rounding or saturation mode. For specific layout requirements, see [cast_layout](../aux_data_types/cast_layout.md).

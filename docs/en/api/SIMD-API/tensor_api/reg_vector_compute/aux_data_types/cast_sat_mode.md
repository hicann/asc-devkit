# cast_sat_mode

<!-- md-trans-meta sourceCommit=68a0a4d6fe3664306d9b3a74c585f91fd4411e41 translatedAt=2026-11-19T02:43:03.655Z pushedAt=2026-11-19T05:54:06.652Z nl=2 -->

## Function Description

Header file path: `"tensor_api/experimental/utils/reg_enum.h"`.

`cast_sat_mode` specifies whether to use saturation mode during [cast](../type_conversion/cast.md) conversion.

## Type Definition

```cpp
enum class cast_sat_mode {
    unknown = -1,
    not_sat,
    sat,
};
```

## Enum Value Description

| Enum Value | Description |
| :-- | :-- |
| unknown | Reserved value used when the conversion type does not support saturation mode configuration. |
| not_sat | Non-saturation mode. When the value is out of range, it is truncated to the destination format, or special values supported by the floating-point format are retained. |
| sat | Saturation mode. When the value is out of range, the result is clamped to the boundary of the destination type. |

For the specific behavior of different type conversions, see [Rounding Mode and Saturation Mode](../type_conversion/rounding_mode.md) and [Saturation and Non-saturation Modes of cast](../type_conversion/cast.md#saturation-and-non-saturation-modes).

# cast_round_mode

<!-- md-trans-meta sourceCommit=63c0b0eaa8a2923ff3c351fb2a0592ad269f94d6 translatedAt=2026-11-19T02:42:46.600Z pushedAt=2026-11-19T05:54:06.650Z nl=2 -->

## Function Description

Header file path: `"tensor_api/experimental/utils/reg_enum.h"`.

`cast_round_mode` specifies the rounding mode for [cast](../type_conversion/cast.md) conversion.

## Type Definition

```cpp
enum class cast_round_mode {
    unknown = -1,
    rint,
    round,
    floor,
    ceil,
    trunc,
    odd,
    hybrid,
};
```

## Enum Value Description

| Enum Value | Description |
| :-- | :-- |
| unknown | No rounding mode specified. Reserved value. |
| rint | Round to the nearest value, with ties rounding to even. |
| round | Round to the nearest value, with ties rounding away from 0. |
| floor | Round toward negative infinity. |
| ceil | Round toward positive infinity. |
| trunc | Round toward 0. |
| odd | Round to the nearest odd value. Currently supported only for `float` to `half` conversion. |
| hybrid | Hybrid rounding. Currently used for `float` or `half` to `hifloat8_t` conversion. |

For the specific behavior of rounding modes and the supported type combinations, see [Rounding Modes and Saturation Modes](../type_conversion/rounding_mode.md) and [Data Type Description of cast](../type_conversion/cast.md#data-type).

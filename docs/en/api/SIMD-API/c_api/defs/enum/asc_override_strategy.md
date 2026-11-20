---
okftype: reference.enum
description: "Saturation control strategy enum, configured and queried by asc_set_saturation_strategy and asc_get_saturation_strategy, determining whether the CAST global saturation configuration takes effect: USE_API uses per-instruction path configuration, USE_GLOBAL uses the global configuration."
tag:
  - roles: [configuration]
  - traits: [saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/basic_api/reg_vector_compute/type_conversion/Cast.md
    kind: related
    condition: "When USE_API is used with per-instruction saturation, see the SatMode and saturation mode effective configuration table on this page"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_get_saturation_strategy.md
    kind: related
    condition: "This interface queries and returns the current strategy represented by this enum"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_saturation_flag.md
    kind: related
    condition: "When USE_GLOBAL is used, the global CAST saturation configured by this interface takes effect in precision conversion"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_saturation_strategy.md
    kind: related
    condition: "This interface uses this enumeration to set the saturation strategy"
version: ">=9.2.0"
version_changes: []
---
# asc_override_strategy

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:10:59.030Z pushedAt=2026-11-18T08:15:34.395Z nl=2 -->

asc_override_strategy is used to represent the saturation strategy, and is used when calling [asc_set_saturation_strategy](../../spr/asc_set_saturation_strategy.md) and [asc_get_saturation_strategy](../../spr/asc_get_saturation_strategy.md). This strategy determines whether the CAST global saturation configuration takes effect.

The header file path is: `"c_api/defs/enum.h"`.

## Specific Definition of the Enum Class

```cpp
enum class asc_override_strategy : uint8_t {
    USE_API = 0,
    USE_GLOBAL = 1
};
```

## Enum Value Details

**Table 1** Enum value description

| Enum value | Description |
| ------ | ----------- |
| USE_API | Configure saturation per instruction/API path. In this case, the global `CAST` saturation configured through [asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md) does not take effect. |
| USE_GLOBAL | Use the global saturation configuration. In this case, the `CAST` mode configured through [asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md) takes effect in precision conversion. |

Typical usage: `USE_GLOBAL` works with CAST saturation for float→int32 dynamic quantization. See the call example in [asc_set_saturation_strategy](../../spr/asc_set_saturation_strategy.md) and the [Cast sample](../../../../../../../examples/02_simd_c_api/03_c_api/02_reg_vector_compute/cast). `USE_API` works with per-instruction saturation. See the `SatMode` and the saturation mode global or per-instruction effect configuration table in [Cast](../../../basic_api/reg_vector_compute/type_conversion/Cast.md).

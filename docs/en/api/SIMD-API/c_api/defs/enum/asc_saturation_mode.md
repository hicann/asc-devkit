---
okftype: reference.enum
description: "Enumeration of mode types on which Saturation Control acts, for asc_set_saturation_flag and asc_get_saturation_flag to toggle saturation by float, FP8, integer, and type conversion scenarios, where the CAST level takes effect only under the global saturation policy."
tag:
  - roles: [configuration]
  - traits: [saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/enum/asc_override_strategy.md
    kind: related
    condition: "The CAST level takes effect only when the saturation policy is USE_GLOBAL"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_get_saturation_flag.md
    kind: related
    condition: "This interface queries the saturation switch for the corresponding scenario according to this enumeration"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_saturation_flag.md
    kind: related
    condition: "This interface specifies the scenario for which saturation is to be toggled using this enumeration"
version: ">=9.2.0"
version_changes: []
---
# asc_saturation_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:13:14.141Z pushedAt=2026-11-18T08:15:34.400Z nl=2 -->

asc_saturation_mode is used to indicate the mode type on which Saturation Control acts, and is used when calling [asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md) and [asc_get_saturation_flag](../../spr/asc_get_saturation_flag.md).

The header file path is: `"c_api/defs/enum.h"`.

## Specific Definition of the Enum Class

```cpp
enum class asc_saturation_mode : uint8_t {
    FLOAT = 48,
    FLOAT8 = 50,
    INT = 53,
    CAST = 59
};
```

## Enum Value Details

**Table 1** Enum value description

| Enum Value | Description |
| ------ | ----------- |
| FLOAT | Saturation control for floating-point computation and some floating-point precision conversion scenarios. Floating-point computation supports the half data type; floating-point precision conversion supports the hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, half, and bfloat16_t data types. |
| FLOAT8 | Saturation control for scenarios related to fp8_e8m0_t, fp8_e5m2_t, and fp8_e4m3fn_t. |
| INT | Saturation/truncation control for integer computation overflow. |
| CAST | Precision conversion saturation control for floating-point-to-integer or integer-to-integer conversion. It takes effect only when the saturation strategy is [asc_override_strategy::USE_GLOBAL](asc_override_strategy.md). |

For the behavioral differences of enabling/disabling saturation in each mode, see the `enable_sat` parameter description in [asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md).

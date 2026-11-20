---
okftype: reference.enum
description: "Enumeration of rounding mode management policies for rounding FP32 to HF32 after Mmad enables HF32 mode, configured via asc_set_hf32_round_mode, including NEAREST_AWAY (round half away from zero), NEAREST_EVEN (round half to even), and the compatibility alias NEAREST_ZERO."
tag:
  - roles: [configuration]
  - traits: [rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/cube_compute/mmad_compute_aux_config/asc_set_hf32_round_mode.md
    kind: related
    condition: "After HF32 mode is enabled, this enum is consumed by this interface"
version: ">=9.2.0"
version_changes:
  - since: "9.2.0"
    kind: enum_value_changed
    scope: "enum_value:asc_hf32_round_mode.NEAREST_ZERO"
    scope_basis: resolved
    note: "NEAREST_ZERO changed from an independent value that rounds to the nearest value with ties toward zero to a compatibility alias equivalent to NEAREST_AWAY (written as NEAREST_ZERO = NEAREST_AWAY, same value retained), and no longer represents an independent rounding behavior; the behavior of the old usage changes accordingly, and new code should use NEAREST_AWAY or NEAREST_EVEN."
---
# asc_hf32_round_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:09:26.086Z pushedAt=2026-11-18T08:15:34.388Z nl=2 -->

asc_hf32_round_mode is used to manage the rounding mode policy for rounding from FP32 to HF32 when the HF32 mode is enabled for Mmad computation. It is used when calling [asc_set_hf32_round_mode](../../cube_compute/mmad_compute_aux_config/asc_set_hf32_round_mode.md).

The header file path is: `"c_api/defs/enum.h"`.

## Specific Definition of the Enum Class

```cpp
enum class asc_hf32_round_mode : uint8_t {
    NEAREST_AWAY = 0,
    NEAREST_EVEN = 1,
    NEAREST_ZERO = NEAREST_AWAY  // Compatible with the old name; the actual behavior is equivalent to NEAREST_AWAY.
};
```

## Enum Value Details

|Enum Value|HF32 Rounding Mode Management Policy|
|------|-----------|
| NEAREST_AWAY | FP32 is rounded to HF32 by rounding to the nearest value, with ties away from zero. |
| NEAREST_EVEN | FP32 is rounded to HF32 by rounding to the nearest value, with ties to even. |
| NEAREST_ZERO | Used for compatibility with the old implementation and not recommended. The actual behavior is equivalent to `NEAREST_AWAY`. |

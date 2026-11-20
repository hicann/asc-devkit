---
okftype: reference.enum
description: "Enumeration that configures the UnitFlag behavior after a cube computation or cube move-out interface is executed. It serves as the type of the unit_flag_mode parameter of related interfaces, distinguishing between disabling, keeping the unit flag bit after enabling, and changing the unit flag bit after enabling."
tag:
  - roles: [configuration]
dependencies: []
version: ">=9.2.0"
version_changes: []
---
# asc_unit_flag_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:14:01.212Z pushedAt=2026-11-18T08:15:34.402Z nl=2 -->

`asc_unit_flag_mode` is used to configure the UnitFlag behavior after a cube computation or cube move-out interface is executed, and serves as the type of the `unit_flag_mode` parameter in related interfaces.

The header file path is: `"c_api/defs/enum.h"`.

## Specific Definition of the Enum Class

```cpp
enum class asc_unit_flag_mode : uint8_t {
    DISABLE = 0,
    ENABLE_KEEP = 2,
    ENABLE_UPDATE = 3
};
```

## Enum Value Details

**Table 1** Enum value description

| Enum Value | Value | Description |
| --- | --- | --- |
| DISABLE | 0 | Disables UnitFlag. |
| ENABLE_KEEP | 2 | Enables UnitFlag. The hardware does not change the unit flag bit after executing the instruction. |
| ENABLE_UPDATE | 3 | Enables UnitFlag. The hardware changes the unit flag bit after executing the instruction. |

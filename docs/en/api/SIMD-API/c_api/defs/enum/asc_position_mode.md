---
okftype: reference.enum
description: "Enum that represents the even/odd position of the source or destination operand in a register, used by the Reg type conversion and fused computation interfaces to select the data position. EVEN and ODD can be passed directly through the ASC_POSITION_EVEN and ASC_POSITION_ODD constants, respectively."
tag:
  - roles: [configuration]
dependencies: []
version: ">=9.2.0"
version_changes: []
---
# asc_position_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:11:32.520Z pushedAt=2026-11-18T08:15:34.396Z nl=2 -->

`asc_position_mode` indicates the even/odd position of the source or destination operand in a register, and is used by the Reg type conversion and fusion computation interfaces to select the data position. The constants `ASC_POSITION_EVEN` and `ASC_POSITION_ODD` can be directly used as the position parameter of the corresponding interfaces.

The header file path is: `"c_api/defs/enum.h"`.

## Specific Definition of the Enum Class

```cpp
enum class asc_position_mode {
    EVEN = 0,
    ODD
};

constexpr std::integral_constant<asc_position_mode, asc_position_mode::EVEN> ASC_POSITION_EVEN;
constexpr std::integral_constant<asc_position_mode, asc_position_mode::ODD> ASC_POSITION_ODD;
```

## Enum Value Details

**Table 1** Enum value description

| enum value | value | Description | corresponding constant |
| --- | --- | --- | --- |
| EVEN | 0 | Selects the even position. | ASC_POSITION_EVEN |
| ODD | 1 | Selects the odd position. | ASC_POSITION_ODD |

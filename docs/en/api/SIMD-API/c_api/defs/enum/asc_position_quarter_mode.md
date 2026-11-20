---
okftype: reference.enum
description: "Enum that represents the quarter position of a source or destination operand, used by the Reg type conversion interface to select the data position. The four DISPERSE_QUARTER values correspond to the ASC_DISPERSE_QUARTER series constants and can be passed directly as interface parameters."
tag:
  - roles: [configuration]
dependencies: []
version: ">=9.2.0"
version_changes: []
---
# asc_position_quarter_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:12:17.556Z pushedAt=2026-11-18T08:15:34.397Z nl=2 -->

`asc_position_quarter_mode` is used to represent the quarter position of a source or destination operand, for the Reg type conversion interface to select the data position. The corresponding `ASC_DISPERSE_*_QUARTER` constants can be passed directly as interface parameters.

The header file path is: `"c_api/defs/enum.h"`.

## Specific Definition of the Enum Class

```cpp
enum class asc_position_quarter_mode {
    DISPERSE_FIRST_QUARTER = 0,
    DISPERSE_SECOND_QUARTER,
    DISPERSE_THIRD_QUARTER,
    DISPERSE_FOURTH_QUARTER
};
```

## Enum Value Details

**Table 1** Enum value description

| enum value | value | Description | corresponding constant |
| --- | --- | --- | --- |
| DISPERSE_FIRST_QUARTER | 0 | Selects the first quarter position. | ASC_DISPERSE_FIRST_QUARTER |
| DISPERSE_SECOND_QUARTER | 1 | Selects the second quarter position. | ASC_DISPERSE_SECOND_QUARTER |
| DISPERSE_THIRD_QUARTER | 2 | Selects the third quarter position. | ASC_DISPERSE_THIRD_QUARTER |
| DISPERSE_FOURTH_QUARTER | 3 | Selects the fourth quarter position. | ASC_DISPERSE_FOURTH_QUARTER |

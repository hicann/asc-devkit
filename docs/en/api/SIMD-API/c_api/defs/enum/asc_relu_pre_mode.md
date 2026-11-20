---
okftype: reference.enum
description: "Enum of in-line ReLU modes when moving data out of the L0C Buffer, used as the type of the relu_pre_mode parameter in the cube move-out interface, distinguishing no execution, normal ReLU, and in-line ReLU executed with Scalar or Vector parameters."
tag:
  - roles: [data_movement_configuration]
  - operations: [relu]
dependencies: []
version: ">=9.2.0"
version_changes: []
---
# asc_relu_pre_mode

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:13:02.694Z pushedAt=2026-11-18T08:15:34.399Z nl=2 -->

`asc_relu_pre_mode` is used to indicate the in-line ReLU mode when L0C Buffer data is moved out, serving as the type of the `relu_pre_mode` parameter in the cube move-out interface.

The header file path is: `"c_api/defs/enum.h"`.

## Specific Definition of the Enum Class

```cpp
enum class asc_relu_pre_mode : uint8_t {
    NONE = 0,
    NORMAL = 1,
    SCALAR = 2,
    VECTOR = 3,
};
```

## Enum Value Details

**Table 1** Enum value description

| Enum Value | Value | Description |
| --- | --- | --- |
| NONE | 0 | Does not perform the in-line ReLU. |
| NORMAL | 1 | Performs the normal ReLU. |
| SCALAR | 2 | Performs the in-line ReLU using the Scalar parameter. |
| VECTOR | 3 | Performs the in-line ReLU using the Vector parameter. |

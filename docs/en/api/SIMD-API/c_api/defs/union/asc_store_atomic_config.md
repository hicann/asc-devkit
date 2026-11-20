---
okftype: reference.union
description: "A union that stores the enable bit and operation type for data movement atomic operations. atomic_type values 0 through 6 indicate disabled or float, half, int16_t, int32_t, int8_t, bfloat16_t types respectively, and atomic_op value 0 indicates summation, for reading by asc_get_store_atomic_config."
tag:
  - roles: [data_movement_configuration]
  - traits: [atomic]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/atomic/scalar_atomic/asc_get_store_atomic_config.md
    kind: related
version: ">=9.0.0"
version_changes: []
---
# asc_store_atomic_config

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:19:34.800Z pushedAt=2026-11-18T08:15:34.420Z nl=2 -->

The header file path is: `"c_api/defs/union.h"`.

The asc_store_atomic_config parameter contains the atomic operation enable bit and the atomic operation type, and is used by the [asc_get_store_atomic_config](../../atomic/scalar_atomic/asc_get_store_atomic_config.md) interface.

## Struct Specific Definition

```cpp
constexpr uint64_t ASC_STORE_ATOMIC_DEFAULT_VALUE = 0x0000000000000000;
union asc_store_atomic_config {
    uint64_t config = ASC_STORE_ATOMIC_DEFAULT_VALUE;
    struct {
        uint64_t atomic_type : 3;
        uint64_t atomic_op : 2;
        uint64_t reserved1 : 59;
    };
};
```

## Field Details

**Table 1** Field description

|Field Name|Field Meaning|
|----------|----------|
| atomic_type | Atomic operation enable bit. Default value: 0.<br>0: No atomic operation.<br>1: Enable the atomic operation, with the data type float.<br>2: Enable the atomic operation, with the data type half.<br>3: Enable the atomic operation, with the data type int16_t.<br>4: Enable the atomic operation, with the data type int32_t.<br>5: Enable the atomic operation, with the data type int8_t.<br>6: Enable the atomic operation, with the data type bfloat16_t.<br>Other values have no specific meaning.|
| atomic_op | Atomic operation type. Default value: 0.<br>0: Sum operation.<br>Other values have no specific meaning. |

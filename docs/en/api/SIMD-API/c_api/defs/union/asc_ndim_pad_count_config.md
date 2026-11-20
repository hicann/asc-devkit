---
okftype: reference.union
description: "Union that provides the number of padding elements in each dimension for asc_set_ndim_pad_count. The 8 uint8_t fields respectively represent the number of elements to pad on the left and right sides of dimensions 1 to 4, for use by asc_ndim_copy_gm2ub. The default values are all 0."
tag:
  - roles: [data_movement_configuration]
  - operations: [copy]
  - traits: [high_dimensional, padding]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_set_ndim_pad_count.md
    kind: related
version: ">=9.0.0"
version_changes: []
---
# asc_ndim_pad_count_config

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:18:57.546Z pushedAt=2026-11-18T08:15:34.417Z nl=2 -->

The header file path is: `"c_api/defs/union.h"`.

asc_ndim_pad_count_config is used in the [asc_set_ndim_pad_count](../../vector_datamove/asc_set_ndim_pad_count.md) interface to set the number of padding elements on the left and right sides of each dimension for the [asc_ndim_copy_gm2ub](../../vector_datamove/asc_ndim_copy_gm2ub.md) interface.

## Structure Definition

```cpp
constexpr uint64_t ASC_DEFAULT_NDIM_PAD_COUNT_CONFIG_VALUE = 0;
union asc_ndim_pad_count_config {
    uint64_t config = ASC_DEFAULT_NDIM_PAD_COUNT_CONFIG_VALUE;
    struct {
        uint8_t loop1_lp_count;
        uint8_t loop1_rp_count;
        uint8_t loop2_lp_count;
        uint8_t loop2_rp_count;
        uint8_t loop3_lp_count;
        uint8_t loop3_rp_count;
        uint8_t loop4_lp_count;
        uint8_t loop4_rp_count;
    };
};
```

## Field Details

**Table 1** Field description

|Field name|Field meaning|
|----------|----------|
| loop1_lp_count | Indicates the number of elements to pad on the left side of dimension 1.<br> unit: elements, value range: [0, 255], default value: 0. |
| loop1_rp_count | Indicates the number of elements to pad on the right side of dimension 1.<br> unit: elements, value range: [0, 255], default value: 0. |
| loop2_lp_count | Indicates the number of elements to pad on the left side of dimension 2.<br> unit: elements, value range: [0, 255], default value: 0. |
| loop2_rp_count | Indicates the number of elements to pad on the right side of dimension 2.<br> unit: elements, value range: [0, 255], default value: 0. |
| loop3_lp_count | Indicates the number of elements to pad on the left side of dimension 3.<br> unit: elements, value range: [0, 255], default value: 0. |
| loop3_rp_count | Indicates the number of elements to pad on the right side of dimension 3.<br> unit: elements, value range: [0, 255], default value: 0. |
| loop4_lp_count | Indicates the number of elements to pad on the left side of dimension 4.<br> unit: elements, value range: [0, 255], default value: 0. |
| loop4_rp_count | Indicates the number of elements to pad on the right side of dimension 4.<br> unit: elements, value range: [0, 255], default value: 0. |

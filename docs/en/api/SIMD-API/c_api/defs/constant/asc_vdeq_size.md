---
okftype: reference.constant
description: "Defines the value and purpose of the constant ASC_VDEQ_SIZE: this constant is fixed at 16, representing the number of elements in the three arrays scale_arr, offset_arr, and sign_mode_arr in asc_set_deq_scale."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_set_deq_scale.md
    kind: related
    condition: "This constant provides the length for the three quantization-related array parameters of asc_set_deq_scale"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The definition location was moved from the implementation-side impl/c_api/instr_impl/npu_arch_2201/utils_impl/utils_impl.h to the public header file include/c_api/defs/constant.h (line 17), with the value still 16; the CANN 9.2.0 Release Notes public header file section explicitly records that ASC_VDEQ_SIZE was moved from the implementation-side definition to c_api/defs/constant.h, and its purpose in existing API prototypes remains unchanged."
---
# ASC_VDEQ_SIZE

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:07:31.324Z pushedAt=2026-11-18T08:15:34.382Z nl=2 -->

ASC_VDEQ_SIZE indicates the number of elements in the scale_arr, offset_arr, and sign_mode_arr arrays in the [asc_set_deq_scale](../../vector_compute/vector_fused/asc_set_deq_scale.md) interface, with a value of 16.

Header file path: `c_api/defs/constant.h`.

## Constant Definition

```cpp
constexpr uint8_t ASC_VDEQ_SIZE = 16;
```

---
okftype: reference.constant
description: "Defines the value and purpose of the constant ASC_C_API_MRGSORT_ELEMENT_LEN: this constant is fixed at 4, indicating the number of queues supported by MrgSort4, and is used as the length of the source queue address array of asc_mrgsort4 and the result array of asc_get_vms4_sr."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_sort/asc_get_vms4_sr.md
    kind: related
    condition: "This constant provides the length for the sorted_num result array of asc_get_vms4_sr"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_sort/asc_mrgsort4.md
    kind: related
    condition: "This constant provides the length for the src queue address array of asc_mrgsort4"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "The definition location was moved from the implementation-side header impl/c_api/instr_impl/npu_arch_2201/utils_impl/utils_impl.h to the public header include/c_api/defs/constant.h (line 16), with the value still 4; the CANN 9.2.0 Release Notes public header section explicitly records that ASC_C_API_MRGSORT_ELEMENT_LEN was moved from the implementation-side definition to c_api/defs/constant.h, and its purpose in existing API prototypes remains unchanged."
---
# ASC_C_API_MRGSORT_ELEMENT_LEN

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T12:07:10.367Z pushedAt=2026-11-18T08:15:34.380Z nl=2 -->

ASC_C_API_MRGSORT_ELEMENT_LEN indicates the number of queues supported by the MrgSort4 operation, with a value of 4. This constant is used for the length of the src array of the [asc_mrgsort4](../../vector_compute/vector_sort/asc_mrgsort4.md) interface, and for the length of the sorted_num array of the [asc_get_vms4_sr](../../vector_compute/vector_sort/asc_get_vms4_sr.md) interface.

Header file path: `c_api/defs/constant.h`.

## Constant Definition

```cpp
constexpr uint8_t ASC_C_API_MRGSORT_ELEMENT_LEN = 4;
```

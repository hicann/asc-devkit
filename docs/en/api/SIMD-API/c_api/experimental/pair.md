# Dual Return Value Structure

<!-- md-trans-meta sourceCommit=ceded3e2df49735f6a8a8ecb1b10a25352b15e43 translatedAt=2026-11-16T15:38:04.331Z pushedAt=2026-11-18T03:12:16.721Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is: `"c_api/experimental/pair.h"`.

This structure is an experimental interface, which may be adjusted or removed in later versions, and backward compatibility is not guaranteed. When compiling, you need to define the `CANN_ASC_USE_EXPERIMENTAL` macro and explicitly include the specified experimental header file.

The dual return value structure is used to carry two register results through a function return value, and contains two members: `first` and `second`. Multiple interfaces can share the same structure, and the specific results stored in the members are defined by the corresponding interfaces.

For the register types used by the members, see [Register Type Definition](../defs/type/data_type_definition.md).

## Structure Definition

The following uses placeholders to describe the naming and member types of the structure, where `<dtype0>` and `<dtype1>` represent type names.

```c
struct asc_<dtype0>_<dtype1>_pair {
    vector_<dtype0> first;
    vector_<dtype1> second;
};
```

### Supported Data Types for dtype0 and dtype1

| dtype0 | dtype1 |
| --- | --- |
| `bool` | `bool` |
| `int4x2_t` | `int4x2_t` |
| `int8_t` | `int8_t` |
| `uint8_t` | `uint8_t` |
| `fp4x2_e2m1_t` | `fp4x2_e2m1_t` |
| `fp4x2_e1m2_t` | `fp4x2_e1m2_t` |
| `hifloat8_t` | `hifloat8_t` |
| `fp8_e8m0_t` | `fp8_e8m0_t` |
| `fp8_e5m2_t` | `fp8_e5m2_t` |
| `fp8_e4m3fn_t` | `fp8_e4m3fn_t` |
| `int16_t` | `int16_t` |
| `uint16_t` | `uint16_t` |
| `half` | `half` |
| `bfloat16_t` | `bfloat16_t` |
| `int32_t` | `bool` |
| `int32_t` | `int32_t` |
| `uint32_t` | `bool` |
| `uint32_t` | `uint32_t` |
| `float` | `float` |

### Typical Example of the Structure

```c
struct asc_int32_t_int32_t_pair {
    vector_int32_t first;
    vector_int32_t second;
};
```

## Field Description

| Field Name | Data Type | Field Description |
| --- | --- | --- |
| first | `vector_<dtype0>` | The first return result. Its specific meaning is defined by the return value description of the corresponding interface. When `dtype0` is `bool`, the type is `vector_bool`. |
| second | `vector_<dtype1>` | The second return result. Its specific meaning is defined by the return value description of the corresponding interface. When `dtype1` is `bool`, the type is `vector_bool`. |

## Constraints

- Before using this type of structure, include the header file: `#include "c_api/experimental/pair.h"`.
- Before use, add the compilation option `-DCANN_ASC_USE_EXPERIMENTAL` to enable the experimental feature.

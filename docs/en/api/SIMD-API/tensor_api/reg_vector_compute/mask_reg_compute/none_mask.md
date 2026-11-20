# none_mask

<!-- md-trans-meta sourceCommit=17055e7d86210ccec07ec8a5fa4d562d0d39d24f translatedAt=2026-10-27T03:54:58.733Z pushedAt=2026-10-27T13:03:35.440Z nl=2 -->

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
- AI Core in Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core in Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `"tensor_api/experimental/arch/vector/mask_reg_compute.h"`, and the namespace is `asc::te::experimental`.

Creates a `reg_tensor<bool>` whose member variable `reg` is an all-invalid mask register matching the element width of `DataType`, equivalent to `make_mask<mask_pattern::none, DataType>()`.

## Function Prototype

```cpp
template <typename DataType>
__simd_callee__ inline reg_tensor<bool> none_mask()
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `DataType` | Input | Template parameter. The supported data types are b8/b16/b32. |

## Return Value

Returns a `reg_tensor<bool>` whose `reg` member is a mask register of type `vector_bool`, with all corresponding elements being invalid data.

## Constraints

- The returned object only guarantees that the `reg` member is valid, and does not initialize the member variable `mask`.
- The maximum number of mask registers is 8. Register data exceeding this limit is written to the reserved 8KUB memory, which may cause performance degradation. The compiler automatically reuses registers and reserved memory whose lifecycles have ended. If both registers and reserved memory have available space, registers are reused first.

## Example

```cpp
auto disabled = asc::te::experimental::none_mask<uint32_t>();
auto value = asc::te::experimental::load(src, asc::te::make_coord(0)).with_mask(disabled);
```

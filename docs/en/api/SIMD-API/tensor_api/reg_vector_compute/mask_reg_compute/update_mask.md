# update_mask

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T03:55:25.817Z pushedAt=2026-10-27T13:03:35.441Z nl=2 -->

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

Generates a valid bit mask of the corresponding length based on the current number of remaining elements to be processed, and automatically subtracts the current vector length from the current number of remaining elements to update the number of remaining elements to be processed.

## Function Prototype

```cpp
template <typename DataType>
__simd_callee__ inline reg_tensor<bool> update_mask(uint32_t& remain)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `DataType` | Input | Template parameter. The supported data types are b8/b16/b32. |
| `remain` | Input/Output | Number of elements. After the call, the number of elements processed by the current vector processing unit (256, 128, and 64 for b8, b16, and b32 modes, respectively) is automatically subtracted.<br>After this API is executed once, `remain = (remain < VL_T) ? 0 : (remain - VL_T)`, where `VL_T` indicates the number of elements of data type T that can be stored in a vector data register with a bit width of VL. |

## Return Value

Returns `reg_tensor<bool>`, whose `reg` member is a mask register of type vector_bool, and whose corresponding elements are valid data.

## Constraints

- The returned object only guarantees that the `reg` member is valid, and does not initialize the member variable `mask`.
- The maximum number of mask registers is 8. Register data exceeding this limit is written to the reserved 8KUB memory, which may cause performance degradation. The compiler automatically reuses registers and reserved memory whose lifecycles have ended. If both registers and reserved memory have available space, registers are reused first.

## Example

```cpp
uint32_t remain = count;
for (uint32_t offset = 0; offset < count; offset += elementsPerReg) {
    auto tailMask = asc::te::experimental::update_mask<float>(remain);
    auto value = asc::te::experimental::load(src, asc::te::make_coord(offset)).with_mask(tailMask);
    asc::te::experimental::store(dst, asc::te::make_coord(offset), value);
}
```

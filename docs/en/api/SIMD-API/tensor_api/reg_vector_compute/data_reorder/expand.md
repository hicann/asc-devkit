# expand

<!-- md-trans-meta sourceCommit=db86c75495b5d041b217977e24030960f4f2178e translatedAt=2026-11-19T02:48:34.375Z pushedAt=2026-11-19T05:54:06.660Z nl=2 -->

## Supported Products

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

## Function Description

The header file path is `"tensor_api/experimental/arch/vector/reg_compute_sel.h"`, and the namespace is `asc::te::experimental`.

Generates prefix count indexes that exclude the current position based on the mask: the 0th element of the result is 0, and the i-th element equals the number of 1s in the valid bits corresponding to the 0th through (i-1)-th elements in `mask.reg`. The mask is interpreted according to the element width of T, and the mask bit corresponding to the last element does not participate in the count of any result element.

For example, when the valid bits corresponding to the first six elements of the mask are `{1, 0, 1, 1, 0, 1}`, the first six elements of the result are `{0, 1, 1, 2, 3, 3}`.

This API generates indexes. It does not receive the data to be expanded, nor does it directly restore compressed data to its original positions. This API takes effect only on the AIV.

## Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> expand(reg_tensor<bool> mask);
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Element type of the result indexes, which must be explicitly specified. For supported types, see [data type](#data-type). |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mask | Input | Source mask, of type [reg_tensor&lt;bool&gt;](../reg_tensor/reg_tensor.md). `mask.reg` participates in the prefix count and is not used as the validity mask of the returned object. |

## Data Type

T supports: int8_t, uint8_t, int16_t, uint16_t, int32_t, and uint32_t.

## Return Value

Returns `reg_tensor<T>`, where `reg` stores the prefix count index and `mask` is `all_mask<T>().reg`, that is, an all-valid mask set according to the result element type.

## Constraints

- `mask.reg` must be initialized and constructed according to the element width of T. `mask.mask` does not participate in the computation.
- The mask of the returned object is an all-valid mask, so all prefix counts can be used directly in subsequent operations. If only the positions where the input mask is 1 are needed, rebind the input mask to the result through [with_mask](../reg_tensor/reg_tensor.md#with_mask).
- Signed result types are still subject to the representable range of the corresponding type. For example, int8_t cannot represent a non-negative count greater than 127. When counts above this range must be preserved, choose an appropriate data type.

## Example

The following code snippet is called in the VF. `selection.reg` is initialized and constructed according to the bit width of the uint16_t element. The mask of the returned object is an all-valid mask of the uint16_t type, so all prefix counts can be used.

```cpp
__simd_callee__ inline asc::te::experimental::reg_tensor<uint16_t> expand_example(
    asc::te::experimental::reg_tensor<bool> selection)
{
    return asc::te::experimental::expand<uint16_t>(selection);
}
```

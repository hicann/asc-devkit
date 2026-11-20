# compress

<!-- md-trans-meta sourceCommit=f081e8db72fcbb3defe09b5d9cd8a4c92372320b translatedAt=2026-10-27T03:40:53.906Z pushedAt=2026-10-27T13:03:35.410Z nl=2 -->

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

The header file path is `"tensor_api/experimental/arch/vector/reg_compute_sel.h"`, and the namespace is `asc::te::experimental`.

Selects elements from the source register according to `src.mask`, places the selected elements contiguously in the low-order bits of the result register in their original order, and sets the remaining elements to 0. For example, if the first four elements of the source data are `{10, 20, 30, 40}` and `src.mask` selects the 0th, 2nd, and 3rd elements while deselecting the rest, the result is `{10, 30, 40, 0, ...}`.

This API takes effect only on the AIV.

## Function Prototype

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> compress(reg_tensor<T> src);
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Element type of the source register and the returned result. For supported types, see [Data Type](#data-type). |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src | Input | Source operand of type [reg_tensor&lt;T&gt;](../reg_tensor/reg_tensor.md). `src.mask` is the selection condition, using the bits corresponding to the element width of T, where 1 selects the corresponding source element and 0 deselects it. |

## Data Type

T supports: int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.

## Return Value

Returns `reg_tensor<T>`, where `reg` holds the compressed result and `mask` directly copies `src.mask`. The number of selected elements is not returned.

## Constraints

- `src.mask` must be constructed and initialized according to the element width of T, and the selected source elements must be initialized. You can bind the selection mask through [with_mask](../reg_tensor/reg_tensor.md#with_mask).
- The `mask` of the returned object directly inherits `src.mask`. If subsequent computation or movement only needs to process the selected elements, you must first rebind the mask of the valid elements that are contiguous in the low bits of the result through [with_mask](../reg_tensor/reg_tensor.md#with_mask), instead of directly reusing the original selection mask.

## Example

The following code snippet is called in VF. `src.reg` and `src.mask` are initialized, where `src.mask` is the selection mask. `output_mask.reg` is constructed according to the element width of T and selects only the valid elements that are contiguous in the low bits. The number of its valid elements equals the number of elements selected in `src.mask`.

```cpp
template <typename T>
__simd_callee__ inline asc::te::experimental::reg_tensor<T> compress_example(
    asc::te::experimental::reg_tensor<T> src,
    const asc::te::experimental::reg_tensor<bool>& output_mask)
{
    auto result = asc::te::experimental::compress(src).with_mask(output_mask);
    return result;
}
```

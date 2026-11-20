# store_scatter

<!-- md-trans-meta sourceCommit=a7a6e47f83e7b403679722537209a9d482178ed2 translatedAt=2026-10-27T04:05:37.569Z pushedAt=2026-10-27T13:03:35.455Z nl=2 -->

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

The header file path is `"tensor_api/experimental/arch/vector/reg_data_store.h"`, and the namespace is `asc::te::experimental`.

`store_scatter` scatters the valid elements in [reg_tensor](../reg_tensor/reg_tensor.md) into a Unified Buffer (UB) Tensor according to the index register.

When `coord` is passed in, `tensor.engine() + tensor.layout()(coord)` is used as the base address; when `coord` is not passed in, the start address of `tensor` is used as the base address.

## Function Prototype

```cpp
template <typename Tensor, typename T, typename Index>
__simd_callee__ inline void store_scatter(
    Tensor& tensor,
    const reg_tensor<T>& src,
    const reg_tensor<Index>& index)

template <typename Tensor, typename Coord, typename T, typename Index>
__simd_callee__ inline void store_scatter(
    Tensor& tensor,
    const Coord& coord,
    const reg_tensor<T>& src,
    const reg_tensor<Index>& index)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| `tensor` | Output | Destination Tensor located in the UB. |
| `coord` | Input | Coordinates of the base address in the logical layout of `tensor`. |
| `src` | Input | Source register. `src.mask` indicates element by element whether to write. |
| `index` | Input | Destination position of each element in `src` relative to the base address, in elements. |

## Data Type

The Tensor element type must be consistent with the `src` element type. The supported data type combinations are as follows:

| Tensor element type/`src` element type | `Index` type |
| --- | --- |
| `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t` | `uint16_t` |
| `int32_t`, `uint32_t`, `float` | `uint32_t` |

Unlisted type combinations cause a compile-time error.

## Return Value

None

## Constraints

- Only Tensors located in the UB are supported, and the base address must be 32B aligned.
- The Tensor element type must be consistent with the `src` element type.
- The data corresponding to the index must be within the valid address range of the UB.
- The valid indexes in `index` must be unique. If duplicate indexes exist, the data ultimately written cannot be determined.
- When the element type of `src` is `int8_t` or `uint8_t`, only the elements at even positions in `src` are valid. That is, the elements at positions 0, 2, 4, ..., 252, and 254 in `src` are scattered out to the destination operand.
- When `coord` is passed, the coordinate dimensionality must match the **Tensor** layout, and the coordinates must not be out of bounds.
- Register data move-out should be performed in an `__simd_vf__` function or an `__simd_callee__` function.

## Example

```cpp
template <typename SrcTensor, typename IndexTensor, typename DstTensor>
__simd_vf__ inline void ScatterData(SrcTensor src, IndexTensor index,
    DstTensor dst, uint32_t repeat_times, uint32_t total_length,
    uint32_t one_repeat_size)
{
    using data_type = typename SrcTensor::data_type;
    uint32_t count = total_length;
    for (uint16_t i = 0; i < repeat_times; ++i) {
        auto mask = asc::te::experimental::update_mask<data_type>(count);
        uint32_t offset = i * one_repeat_size;
        const auto coord = asc::te::make_coord(offset);
        auto src_reg = asc::te::experimental::load(src, coord).with_mask(mask);
        auto index_reg = asc::te::experimental::load(index);
        asc::te::experimental::store_scatter(dst, coord, src_reg, index_reg);
    }
}
```

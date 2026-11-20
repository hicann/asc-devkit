# slice

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T04:23:29.841Z pushedAt=2026-10-27T13:03:35.490Z nl=2 -->

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

The header file path is `tensor_api/tensor.h`.

slice is used to slice a sub-tensor from a tensor at the specified coordinates. Based on the input start coordinates coord and the slice description information info, it computes the new start storage position and constructs the corresponding sub-tensor, which retains the same storage position type as the original tensor.

## Function Prototype

```cpp
template <typename Tensor, typename Coord, typename Info>
__aicore__ inline decltype(auto) slice(Tensor&& tensor, const Coord& coord, const Info& info)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Type | Description |
|--------|------|------|
| Tensor | Input | Tensor type, usually global_tensor<...> or local_tensor<...>. |
| Coord | Input | Coordinate type, usually a coord<...> tuple type. |
| Info | Input | Slice description type, which can be a shape type or a layout type. When a shape is passed, it means the sub-tensor is sliced according to the given shape; when a layout is passed, it means the sub-tensor is sliced according to the shape information in the layout while preserving the original layout mode and stride structure. |

**Table 2**  Parameters

| Parameter | Type | Description |
|--------|------|------|
| tensor | Input | Tensor object to be sliced. |
| coord | Input | Start coordinate of the slice. |
| info | Input | Slice description information. A shape or a layout can be passed. |

## Return Value

Returns the sub-tensor object starting from the position specified by coord. The returned result contains the new layout information, and Engine points to the start position after slicing.

## Constraints

- The tuple structure of coord must be consistent with the layout.shape() structure of the current tensor.
- When info is a shape: info must be a two-dimensional shape; the layout of the current tensor must be two-dimensional or four-dimensional.
- When info is a layout, info must have the same dimension as the layout of the current tensor.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr uint64_t gm_addr = 128;

// Example 1: Slice by shape.
auto layout1 = make_layout(make_shape(10, 20), make_stride(20, 1));
auto tensor1 = make_tensor(make_mem_ptr<location::gm, float>(gm_addr), layout1);
auto sub1 = slice(tensor1, make_coord(2, 4), make_shape(4, 8));
// The start coordinate is (2, 4), and the sub-tensor shape is (4, 8).

// Example 2: Slice by layout.
auto layout2 = make_layout(make_shape(10, 20), make_stride(20, 1));
auto tensor2 = make_tensor(make_mem_ptr<location::gm, float>(gm_addr), layout2);
auto sub2 = slice(tensor2, make_coord(1, 2), make_layout(make_shape(3, 5), make_stride(20, 1)));
// Extract a sub-tensor with shape (3, 5) starting from (1, 2), preserving the stride structure.

// Example 3: Equivalent to calling the member function.
auto sub3 = tensor2.slice(make_coord(1, 2), make_shape(3, 5));
// sub3 is equivalent to slice(tensor2, make_coord(1, 2), make_shape(3, 5)).
```

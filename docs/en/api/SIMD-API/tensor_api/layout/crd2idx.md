# crd2idx

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T02:25:18.780Z pushedAt=2026-10-27T13:03:35.299Z nl=2 -->

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

**crd2idx** computes the one-dimensional linear index corresponding to a coordinate based on coord, shape, and stride. When the input is a nested tuple, the API recursively computes the offset of each dimension following the same hierarchical structure and sums them up.

## Function Prototype

```cpp
template <typename Coord, typename Shape, typename Stride>
__aicore__ inline constexpr auto crd2idx(
    const Coord& coord, const Shape& shape, const Stride& stride)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| Coord | Input | Coordinate type, which can be an integer, a compile-time integer constant, or a nested tuple. |
| Shape | Input | Shape type, which can be an integer, a compile-time integer constant, or a nested tuple. |
| Stride | Input | Stride type, whose structure must match that of shape. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| coord | Input | Coordinate to be converted. |
| shape | Input | Shape that describes the size of each dimension. |
| stride | Input | Stride that describes the step of each dimension. |

## Return Value

Returns the one-dimensional linear index corresponding to coord. The return type is deduced from the computation result of coord and stride.

## Constraints

- When coord, shape, and stride are tuples, the number of top-level elements must be consistent, and the nested structures must correspond level by level.
- The coordinate of each dimension in coord must fall within the valid range described by shape.
- The unit of stride must be consistent with the unit used for target address computation.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto tensor_shape = make_shape(8, 16);
auto tensor_stride = make_stride(16, 1);
auto tensor_coord = make_coord(2, 3);

auto index = crd2idx(tensor_coord, tensor_shape, tensor_stride);
// index = 2 * 16 + 3 = 35
```

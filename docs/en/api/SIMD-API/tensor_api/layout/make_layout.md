# make_layout

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T02:32:21.852Z pushedAt=2026-10-27T13:03:35.307Z nl=2 -->

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

make_layout constructs a layout object based on shape and stride, describing the shape of a data tensor in logical dimensions and the stride information corresponding to each dimension.

make_layout supports the following two construction methods:

- Pass in shape and stride to directly construct a layout according to the layout information specified by the user.
- Pass in only shape, and the API automatically derives the stride based on shape to construct a compact layout. When shape is a one-dimensional or multi-dimensional ordinary tuple, the stride is derived in contiguous row-major storage order.

## Function Prototype

```cpp
template <typename Shape, typename Stride>
__aicore__ inline constexpr auto make_layout(const Shape& shape, const Stride& stride)

template <typename Shape>
__aicore__ inline constexpr auto make_layout(const Shape& shape)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Type | Description |
| :------- | :--- | :--- |
| Shape | Input | The type of shape, which must be a tuple type. |
| Stride | Input | The type of stride, which must be a tuple type. This template parameter is included only in the two-parameter overload. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :------- | :-------- | :--- |
| shape | Input | Tuple object that describes the logical shape, which can be constructed by [make_shape](./make_shape.md). The unit is the number of elements. |
| stride | Input | Tuple object that describes the stride of each dimension, which can be constructed by [make_stride](./make_stride.md). The unit is the number of elements. |

## Return Value

- Returns a layout<Shape, Stride> object.

## Constraints

The layout type constructed by make_layout does not explicitly carry LayoutPattern and layout_trait information. For standard layouts whose shape and stride type structures are recognizable, LayoutPattern can be automatically derived through [get_layout_pattern](../utils/get_layout_pattern.md). When derivation is not possible or the complete type information needs to be explicitly specified, use [make_pattern_layout](make_pattern_layout.md).

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// Example 1: Explicitly specify shape and stride.
auto layout1 = make_layout(make_shape(8, 16), make_stride(16, 1));

// Example 2: Specify only shape, and automatically derive the stride of the contiguous layout.
auto layout2 = make_layout(make_shape(8, 16, 32));
// The derived stride is (16 * 32, 32, 1) = (512, 32, 1).

// Example 3: Nested shape automatically derives nested stride.
auto layout3 = make_layout(make_shape(make_shape(2, 4), make_shape(8, 16)));
// The derived stride is a nested structure used to describe FrameLayout.
```

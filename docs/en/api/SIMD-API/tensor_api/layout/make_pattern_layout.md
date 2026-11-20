# make_pattern_layout

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T02:33:43.618Z pushedAt=2026-10-27T13:03:35.311Z nl=2 -->

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

make_pattern_layout constructs a layout object with layout pattern information based on shape, stride, LayoutPattern, and Trait.

Compared with [make_layout](./make_layout.md), make_pattern_layout not only stores shape and stride but also attaches the layout pattern LayoutPattern and Trait information to the layout type.

Trait is used to describe layout features such as the C0 granularity. When the `layout_trait<T, C0>` or `layout_trait_default<T>` form is used, T serves only as a compatibility placeholder or as the basis for C0 deduction, and is not stored as the data type information of the tensor.

## Function Prototype

```cpp
template <typename LayoutPattern, typename Trait, typename Shape, typename Stride>
__aicore__ inline constexpr auto make_pattern_layout(const Shape& shape, const Stride& stride)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Type | Description |
| :------- | :--- | :--- |
| LayoutPattern | Input | Layout pattern type, used to mark the format information of the layout. |
| Trait | Input | Layout trait type, used to mark layout features such as the C0 granularity. |
| Shape | Input | Type of shape, which must be a tuple type. |
| Stride | Input | Type of stride, which must be a tuple type. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :------- | :-------- | :--- |
| shape | Input | Tuple object that describes the logical shape, which can be constructed by [make_shape](./make_shape.md). The unit is the number of elements. |
| stride | Input | Tuple object that describes the stride of each dimension, which can be constructed by [make_stride](./make_stride.md). The unit is the number of elements. |

## Return Value

Returns an object of type `layout<Shape, Stride, Std::tuple<LayoutPattern, Trait>>`.

## Constraints

- shape and stride must meet the layout construction requirements, that is, both must be tuple types.
- LayoutPattern must be a valid layout pattern type, such as nz_layout_ptn, zn_layout_ptn, nd_layout_ptn, dn_layout_ptn, and so on.
- Trait should match LayoutPattern and the subsequent usage scenario; otherwise, subsequent format check or routing may fail.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// Example1: Construct a layout with the NZ layout pattern.
auto layout1 = make_pattern_layout<nz_layout_ptn, layout_trait_default<>>(
    make_shape(make_shape(16, 2), make_shape(16, 4)),
    make_stride(make_stride(16, 256), make_stride(1, 512))
);

// Example2: Construct a layout with the ND layout pattern.
auto layout2 = make_pattern_layout<nd_layout_ptn, layout_trait<Std::ignore_t, _1>>(
    make_shape(32, 64),
    make_stride(64, 1)
);
```

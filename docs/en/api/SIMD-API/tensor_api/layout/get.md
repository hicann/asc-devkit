# get

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T02:27:26.913Z pushedAt=2026-10-27T13:03:35.302Z nl=2 -->

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

get provides two types of capabilities:

1. For a Tuple, extracts an element by positional index or extracts nested elements layer by layer.
2. For a layout, extracts the corresponding sub-tuples from shape and stride respectively, and then combines them into a new sub-layout.

## Function Prototype

```cpp
template <size_t I, size_t... Is, typename Tuple>
__aicore__ inline constexpr auto get(Tuple&& t)

template <size_t... Is, typename Layout>
__aicore__ inline constexpr auto get(const Layout& layout)
```

## Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| Tuple | Input | Tuple. Both shape and stride are aliases of Tuple. |
| I | size_t | First index in the tuple overload. |
| Is... | size_t | Remaining index sequence, used for compile-time recursive selection of the substructure of a tuple or layout. |
| Layout | Input | Type of the layout object. |

**Table 2**  Parameters

| Parameter | Type | Description |
|--------|------|------|
| t | Input | Tuple object from which elements are to be extracted. |
| layout | Input | Layout used to describe the tensor layout. |

## Return Value

- For a Tuple input, returns the element extracted layer by layer along the index chain.
- For a layout input, returns a new sub-layout object composed of the extracted shape and stride.

## Constraints

Is... must be an index within the valid range.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto t = Std::make_tuple(1, Std::make_tuple(2, 3));

auto v0 = get<0>(t);
// v0 = 1

auto v1 = get<1, 0>(t);
// v1 = 2

auto layout = make_layout(make_shape(make_shape(10, 20), make_shape(20, 30)), make_stride(make_stride(10, 200), make_stride(1, 4000)));

// Extract the first dimension.
auto sub_layout = get<1>(layout);
// sub_layout  = make_layout(make_shape(20, 30), make_stride(1, 4000));
```

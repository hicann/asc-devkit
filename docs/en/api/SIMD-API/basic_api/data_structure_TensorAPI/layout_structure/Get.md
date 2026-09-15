# Get

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:47:33.997Z -->

> [!NOTE] Note
> This API is an experimental API and may be adjusted or improved in later versions. Backward compatibility is not guaranteed. Developers are advised to keep track of updates in later versions.

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: Not supported
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

## Description

The header file to include is: `#include "tensor_api/tensor.h"`.

**Get** provides two types of capabilities:

1. For a **Tuple**, extracts elements by positional index or extracts nested elements layer by layer.
2. For a **Layout**, extracts the sub-tuples at the corresponding positions from **shape** and **stride** respectively, and then combines them into a new sub-**Layout**.

## Prototype

```cpp
template <size_t... Is, typename Tuple>
__aicore__ inline constexpr auto Get(Tuple&& t)

template <size_t... Is, typename Shape, typename Stride>
__aicore__ inline constexpr auto Get(const Layout<Shape, Stride>& layout)
```

## Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| Shape | Input | Type of the shape that constitutes the Layout, that is, the tuple type. |
| Stride | Input | Type of the stride that constitutes the Layout, that is, the tuple type. |
| Tuple | Input | Tuple. Both Shape and Stride are aliases of Tuple. |
| Is... | size_t | Index sequence used for compile-time recursive selection of the substructures of shape and stride. |

**Table 2** Parameter description

  | Parameter | Type | Description |
|--------|------|------|
| layout | Input | Layout used to describe the layout of a tensor. |

## Return Value

- For a **Tuple** input, returns the element extracted layer by layer along the index chain.
- For a **Layout** input, returns a new sub-**Layout** object whose **ShapeType** and **StrideType** are the extracted subtypes.

## Constraints

**Is...** must be an index within the valid range.

## Example

```cpp
using namespace AscendC::Te;

auto t = Std::make_tuple(1, Std::make_tuple(2, 3));

auto v0 = Get<0>(t);
// v0 = 1

auto v1 = Get<1, 0>(t);
// v1 = 2

auto layout = MakeLayout(MakeShape(MakeShape(10, 20), MakeShape(20, 30)), MakeStride(MakeStride(10, 200), MakeStride(1, 4000)));

// Extract the first dimension.
auto subLayout = Get<1>(layout);
// subLayout  = MakeLayout(MakeShape(20, 30), MakeStride(1, 4000));
```

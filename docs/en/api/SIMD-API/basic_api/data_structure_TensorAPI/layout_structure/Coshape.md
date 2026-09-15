# Coshape

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:46:04.844Z -->

> [!NOTE]
> This API is experimental and may be adjusted or improved in later versions. Backward compatibility is not guaranteed. Developers should pay attention to updates in later versions during use.

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

## Description

The header file to include is `#include "tensor_api/tensor.h"`.

**Coshape** represents the scalar size of the **Layout** codomain interval, that is, the size of the interval formed by all possible index values after logical coordinates are mapped to a one-dimensional index. Its calculation formula is as follows:

```cpp
Coshape = Σ( (shape[i] - 1) × stride[i] ) + 1
```

## Prototype

```cpp
template <size_t... Is, typename Shape, typename Stride>
__aicore__ inline constexpr auto Coshape(const Layout<Shape, Stride>& layout)
```

## Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| Shape | Input | Type of the shape that constitutes the Layout, that is, a tuple type. |
| Stride | Input | Type of the stride that constitutes the Layout, that is, a tuple type. |
| Is... | size_t... | Index sequence that specifies the sub-dimension range. Computation is performed only on the selected dimensions. |

**Table 2** Parameter description

  | Parameter | Type | Description |
|--------|------|------|
| layout | Input | Layout describes the layout of a tensor. |

## Return Value

Returns the shape of the codomain of the **Layout**.

## Constraints

**Is...** must be an index within the valid range.

## Example

```cpp
using namespace AscendC::Te;

// Example 1: Basic computation.
auto layout = MakeLayout(MakeShape(10, 20), MakeStride(1, 100));
auto coshape = Coshape(layout);
// (10-1)*1 + (20-1)*100 + 1 = 9 + 1900 + 1 = 1910

// Example 2: Specify a sub-dimension.
auto coshape0 = Coshape<0>(layout);
// (10-1)*1 + 1 = 9 + 1 = 10

auto coshape1 = Coshape<1>(layout);
// (20-1)*100 + 1 = 1900 + 1 = 1901
```

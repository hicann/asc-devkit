# coshape

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T02:24:23.338Z pushedAt=2026-10-27T13:03:35.296Z nl=2 -->

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

coshape represents the scalar size of the codomain interval of a layout, that is, the size of the interval formed by all possible index values after logical coordinates are mapped to a one-dimensional index. Its calculation formula is:

```cpp
coshape = Σ( (shape[i] - 1) × stride[i] ) + 1
```

## Function Prototype

```cpp
template <size_t... Is, typename Layout>
__aicore__ inline constexpr auto coshape(const Layout& layout)
```

## Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| Is... | size_t... | Index sequence that specifies the sub-dimension range. Only the selected dimensions are calculated. |
| Layout | Input | Type of the layout object. |

**Table 2**  Parameters

  | Parameter | Type | Description |
|--------|------|------|
| layout | Input | Layout used to describe the tensor layout. |

## Return Value

Returns the shape of the codomain of the layout.

## Constraints

Is... must be an index within the valid range.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// Example 1: Basic computation.
auto layout = make_layout(make_shape(10, 20), make_stride(1, 100));
auto coshape_result = coshape(layout);
// (10-1)*1 + (20-1)*100 + 1 = 9 + 1900 + 1 = 1910

// Example 2: Specifying sub-dimensions.
auto coshape0_result = coshape<0>(layout);
// (10-1)*1 + 1 = 9 + 1 = 10

auto coshape1_result = coshape<1>(layout);
// (20-1)*100 + 1 = 1900 + 1 = 1901
```

# cosize

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T02:24:52.820Z pushedAt=2026-10-27T13:03:35.297Z nl=2 -->

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

cosize indicates the span size of the layout codomain, that is, the length of the interval in which all possible index values lie after logical coordinates are mapped to a one-dimensional index by the layout. cosize and coshape are equivalent in both flat and nested layout scenarios.

## Function Prototype

```cpp
template <size_t... Is, typename Layout>
__aicore__ inline constexpr auto cosize(const Layout& layout)
```

## Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| Is... | size_t | Index sequence. The sub-dimension range can be specified through the template parameter Is.... It is used to recursively select the substructure of shape and stride at compile time. |
| Layout | Input | Type of the layout object. |

**Table 2**  Parameters

  | Parameter | Type | Description |
|--------|------|------|
| layout | Input | Layout used to describe the tensor layout. |

## Return Value

Returns the number of elements in the codomain space.

## Constraints

Is... must be an index within the valid range.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// Example 1: Flat layout, cosize = coshape.
auto layout = make_layout(make_shape(10, 20), make_stride(1, 100));
auto cosize_result = cosize(layout);
// coshape = (10-1)*1 + (20-1)*100 + 1 = 1910
// cosize = tuple_size(1910) = 1910

// Example 2: Specify sub-dimensions.
auto cosize0_result = cosize<0>(layout);
// coshape<0> = (10-1)*1 + 1 = 10
// cosize<0> = tuple_size(10) = 10
```

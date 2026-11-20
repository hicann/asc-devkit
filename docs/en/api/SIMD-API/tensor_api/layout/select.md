# select

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T02:37:29.258Z pushedAt=2026-10-27T13:03:35.318Z nl=2 -->

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

Selects the specified dimensions of the layout's shape and stride to form a new layout object and returns it.

## Function Prototype

```cpp
template <size_t... Is, typename Layout>
__aicore__ inline constexpr auto select(const Layout& layout)
```

## Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| Is... | size_t | Index sequence used to recursively select the substructures of shape and stride at compile time. |
| Layout | Input | Type of the layout object. |

**Table 2**  Parameters

  | Parameter | Type | Description |
|--------|------|------|
| layout | Input | Layout used to describe the tensor layout. |

## Return Value

Returns the sub-layout object.

## Constraints

The index Is... must be within the valid range.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto shape = make_shape(10, 20, 30);
auto layout = make_layout(shape);

// Select the 0th and 1st dimensions.
auto sub_layout = select<0, 1>(layout);
```

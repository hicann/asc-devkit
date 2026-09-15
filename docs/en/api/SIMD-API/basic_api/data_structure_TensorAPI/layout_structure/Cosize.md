# Cosize

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:47:12.332Z -->

> [!NOTE]
> This is an experimental API that may be adjusted or improved in subsequent versions, and backward compatibility is not guaranteed. Developers are advised to keep track of updates in subsequent versions.

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

**Cosize** represents the span size of the **Layout** codomain, that is, the length of the interval in which all possible index values lie after logical coordinates are mapped to one-dimensional indices by the **Layout**. **Cosize** and **Coshape** are equivalent in both flattened and nested **Layout** scenarios.

## Function Prototype

```cpp
template <size_t... Is, typename Shape, typename Stride>
__aicore__ inline constexpr auto Cosize(const Layout<Shape, Stride>& layout)
```

## Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| Shape | Input | Type of the shape that constitutes the Layout, that is, a tuple type. |
| Stride | Input | Type of the stride that constitutes the Layout, that is, a tuple type. |
| Is... | size_t | Index sequence. The sub-dimension range can be specified through the template parameter Is.... It is used to recursively select the substructures of shape and stride at compile time. |

**Table 2** Parameter description

  | Parameter | Type | Description |
|--------|------|------|
| layout | Input | Layout is used to describe the tensor layout. |

## Return Value

Returns the number of elements in the codomain space.

## Constraints

**Is...** must be an index within the valid range.

## Example

```cpp
using namespace AscendC::Te;

// Example 1: Flat layout, Cosize = Coshape.
auto layout = MakeLayout(MakeShape(10, 20), MakeStride(1, 100));
auto cosize = Cosize(layout);
// Coshape = (10-1)*1 + (20-1)*100 + 1 = 1910
// Cosize = TupleSize(1910) = 1910

// Example 2: Specify a sub-dimension.
auto cosize0 = Cosize<0>(layout);
// Coshape<0> = (10-1)*1 + 1 = 10
// Cosize<0> = TupleSize(10) = 10
```

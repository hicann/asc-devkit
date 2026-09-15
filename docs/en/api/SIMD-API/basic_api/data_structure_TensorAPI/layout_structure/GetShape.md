# GetShape

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:48:08.997Z -->

> [!NOTE]Note
> This API is an experimental API. It may be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers should pay attention to updates in later versions during use.

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

`GetShape` is used to extract the Shape part from a Layout:

- **Without specifying the template parameter Is...**: Returns the complete Shape tuple.
- **With the template parameter Is... specified**: Selects the element at the corresponding index from the top-level tuple of the Shape. When a single index is specified, that element is returned; when multiple indices are specified, a new tuple composed of these top-level elements is returned.

## Prototype

```cpp
template <size_t... Is, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto GetShape(const Layout<ShapeType, StrideType>& layout)

template <size_t... Is, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto GetShape(Layout<ShapeType, StrideType>& layout)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| -------- | ----------- | ------ |
| Is... | Input | Optional. Used to select one or more indices in the top-level tuple of Shape. |
| ShapeType | Input | Shape type of Layout. |
| StrideType | Input | Stride type of Layout. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| -------- | ----------- | ------ |
| layout | Input | Layout object. |

## Return Value

Returns a **Shape** object (or its substructure) that describes the tensor shape.

## Constraints

**Is...** must be within the valid range of the top-level **tuple** of **Shape**.

## Example

```cpp
using namespace AscendC::Te;

auto layout = MakeLayout(MakeShape(10, 20, 30));

// Without specifying Is..., return the complete shape tuple.he complete shape tuple.
auto fullShape = GetShape(layout);           // (10, 20, 30)

// Specify a single index to return the corresponding element.urn the corresponding element.
auto s0 = GetShape<0>(layout);              // 10

// Specify multiple indices to form a new tuple.a new tuple.
auto s01 = GetShape<0, 1>(layout);          // (10, 20)
```

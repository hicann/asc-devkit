# GetStride

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:48:38.177Z -->

> [!NOTE]
> This API is experimental and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Developers are advised to keep track of updates in subsequent versions.

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

The header file to be included is `#include "tensor_api/tensor.h"`.

**GetStride** extracts the **Stride** part from **Layout**:

- **Without specifying the template parameter Is...**: Returns the complete **Stride** tuple.
- **With the template parameter Is... specified**: Selects the elements at the corresponding indices from the top-level **Stride** tuple. When a single index is specified, that element is returned; when multiple indices are specified, a new tuple composed of these top-level elements is returned.

## Prototype

```cpp
template <size_t... Is, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto GetStride(const Layout<ShapeType, StrideType>& layout)

template <size_t... Is, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto GetStride(Layout<ShapeType, StrideType>& layout)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| -------- | ----------- | ------ |
| **Is...** | Input | Optional. Used to select one or more indices in the top-level tuple of Stride. |
| **ShapeType** | Input | Shape type of Layout. |
| **StrideType** | Input | Stride type of Layout. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| -------- | ----------- | ------ |
| **layout** | Input | Layout object. |

## Return Value

Returns a **Stride** object (or its substructure) that describes the memory access stride.

## Constraints

**Is...** must be within the valid range of the top-level **tuple** of **Stride**.

## Example

```cpp
using namespace AscendC::Te;

auto layout = MakeLayout(MakeShape(10, 20, 30), MakeStride(1, 10, 200));

// Without specifying Is..., return the complete stride tuple.omplete stride tuple.
auto fullStride = GetStride(layout);         // (1, 10, 200)

// Specify a single index to return the corresponding element. the corresponding element.
auto s0 = GetStride<0>(layout);             // 1

// Specify multiple indexes to form a new tuple.ew tuple.
auto s01 = GetStride<0, 1>(layout);         // (1, 10)
```

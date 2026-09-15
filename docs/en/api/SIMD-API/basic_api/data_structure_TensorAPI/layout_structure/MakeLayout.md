# MakeLayout

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:53:06.976Z -->

> [!NOTE]
> This API is experimental and may be adjusted or improved in later versions, with no guarantee of backward compatibility. Developers are advised to keep track of updates in later versions during use.

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

**MakeLayout** constructs a **Layout** object based on **Shape** and **Stride**, describing the shape of a data tensor in its logical dimensions and the stride information corresponding to each dimension.

**MakeLayout** supports the following two construction methods:

- Pass in **shape** and **stride** to directly construct a **Layout** according to the layout information specified by the user.
- Pass in only **shape**, and the API automatically derives the stride based on **shape** to construct a compact layout. When **shape** is a one-dimensional or multi-dimensional ordinary tuple, the stride is derived using contiguous row-major storage.

## Function Prototype

```cpp
template <typename T, typename U>
__aicore__ inline constexpr auto MakeLayout(const T& shape, const U& stride)

template <typename ShapeType>
__aicore__ inline constexpr auto MakeLayout(const ShapeType& shape)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Type | Description |
| :------- | :--- | :--- |
| T | Input | Type of **shape**, required to be a tuple type. |
| U | Input | Type of **stride**, required to be a tuple type. |
| ShapeType | Input | Type of **shape** in the single-parameter overload, required to be a tuple type. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| :------- | :-------- | :--- |
| shape | Input | Tuple object that describes the logical shape, which can be constructed by [MakeShape](./MakeShape.md). |
| stride | Input | Tuple object that describes the stride of each dimension, which can be constructed by [MakeStride](./MakeStride.md). |

## Return Value

- Returns a **Layout<Shape, Stride>** object.

## Constraints

The **Layout** type constructed by **MakeLayout** does not explicitly carry **LayoutPattern** and **LayoutTrait** information. For standard layouts whose **Shape** and **Stride** type structures are recognizable, **LayoutPattern** can be automatically inferred through [GetLayoutPattern](../utils/GetLayoutPattern.md). When inference is not possible or the complete type information needs to be explicitly specified, use [MakePatternLayout](../layout_structure/MakePatternLayout.md).

## Example

```cpp
using namespace AscendC::Te;

// Example 1: Explicitly specify Shape and Stride.
auto layout1 = MakeLayout(MakeShape(8, 16), MakeStride(16, 1));

// Example 2: Specify only Shape, and automatically derive the Stride of the contiguous layout.
auto layout2 = MakeLayout(MakeShape(8, 16, 32));
// The derived stride is (16 * 32, 32, 1) = (512, 32, 1).

// Example 3: Automatically derive the nested Stride from the nested Shape.
auto layout3 = MakeLayout(MakeShape(MakeShape(2, 4), MakeShape(8, 16)));
// The derived stride is a nested structure used to describe FrameLayout.
```

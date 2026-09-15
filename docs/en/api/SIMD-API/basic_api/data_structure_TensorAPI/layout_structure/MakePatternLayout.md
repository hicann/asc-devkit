# MakePatternLayout

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:54:16.340Z -->

> [!NOTE] Note
> This is an experimental API and may be adjusted or improved in subsequent versions. Backward compatibility is not guaranteed. Developers are advised to keep track of updates in subsequent versions.

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

The header file to be included is `#include "tensor_api/tensor.h"`.

**MakePatternLayout** is used to construct a **Layout** object with layout pattern information based on **Shape**, **Stride**, **LayoutPattern**, and **TraitType**.

Compared with [MakeLayout](./MakeLayout.md), **MakePatternLayout** not only stores **Shape** and **Stride**, but also attaches the layout pattern **LayoutPattern** and **Trait** information to the **Layout** type.

## Prototype

```cpp
template <typename LayoutPattern, typename TraitType, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto MakePatternLayout(const ShapeType& shape, const StrideType& stride)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Type | Description |
| :------- | :--- | :--- |
| LayoutPattern | Input | Layout pattern type, used to mark the format information of the Layout. |
| TraitType | Input | Layout trait type, used to mark additional attributes such as the element type and C0 granularity. |
| ShapeType | Input | Type of shape, which must be a tuple type. |
| StrideType | Input | Type of stride, which must be a tuple type. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| :------- | :-------- | :--- |
| shape | Input | Tuple object that describes the logical shape, which can be constructed by [MakeShape](./MakeShape.md). |
| stride | Input | Tuple object that describes the stride of each dimension, which can be constructed by [MakeStride](./MakeStride.md). |

## Return Value

Returns a **Layout**<**ShapeType**, **StrideType**, **Std::tuple**<**LayoutPattern**, **TraitType**>> object.

## Constraints

- **shape** and **stride** must meet the construction requirements of **Layout**, that is, both must be tuple types.
- **LayoutPattern** must be a valid layout pattern type, such as **NZLayoutPtn**, **ZNLayoutPtn**, **NDLayoutPtn**, and **DNLayoutPtn**.
- **TraitType** must match **LayoutPattern** and the subsequent usage scenarios; otherwise, subsequent format checking or routing may fail.

## Example

```cpp
using namespace AscendC::Te;

// Example 1: Construct a Layout with the NZ layout pattern.
auto layout1 = MakePatternLayout<NZLayoutPtn, LayoutTraitDefault<>>(
    MakeShape(MakeShape(16, 2), MakeShape(16, 4)),
    MakeStride(MakeStride(16, 256), MakeStride(1, 512))
);

// Example 2: Construct a Layout with the ND layout pattern.
auto layout2 = MakePatternLayout<NDLayoutPtn, LayoutTrait<Std::ignore_t, _1>>(
    MakeShape(32, 64),
    MakeStride(64, 1)
);
```

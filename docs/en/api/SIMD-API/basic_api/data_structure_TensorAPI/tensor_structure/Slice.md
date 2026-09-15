# Slice

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T18:04:16.921Z -->

> [!NOTE] Note
> This API is experimental and may be adjusted or improved in subsequent versions. Backward compatibility is not guaranteed. Developers are advised to keep track of updates in subsequent versions.

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

The header file to be included is: `#include "tensor_api/tensor.h"`.

**Slice** is used to slice a sub-tensor from a tensor at specified coordinates. Based on the input start coordinates **coord** and the slice description information **info**, it calculates the new start storage location and constructs the corresponding sub-tensor, which retains the same storage location type as the original tensor.

## Prototype

```cpp
template <typename Tensor, typename Coord, typename Info>
__aicore__ inline constexpr decltype(auto) Slice(Tensor&& tensor, const Coord& coord, const Info& info)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Type | Description |
|--------|------|------|
| Tensor | Input | Tensor type, usually **GlobalTensor<...>** or **LocalTensor<...>**. |
| Coord | Input | Coordinate type, usually a **Coord<...>** tuple type. |
| Info | Input | Slice description type, which can be a **Shape** type or a **Layout** type. When **Shape** is passed, it indicates that the sub-tensor is sliced according to the given shape. When **Layout** is passed, it indicates that the sub-tensor is sliced according to the shape information in **Layout** while preserving the original layout mode and **Stride** structure. |

**Table 2** Parameter description

| Parameter | Type | Description |
|--------|------|------|
| tensor | Input | Tensor object to be sliced. |
| coord | Input | Starting coordinate of the slice. |
| info | Input | Slice description information. Either **Shape** or **Layout** can be passed. |

## Return Value

Returns the sub-tensor object starting from the position specified by **coord**. The returned result contains new **Layout** information, and **Engine** points to the starting position after slicing.

## Constraints

- The tuple structure of **coord** must be consistent with the **Layout.Shape()** structure of the current **Tensor**.
- When **info** is **Shape**: **info** must be a two-dimensional **Shape**; the **Layout** of the current **Tensor** must be two-dimensional or four-dimensional.
- When **info** is **Layout**, **info** must have the same number of dimensions as the **Layout** of the current **Tensor**.

## Example

```cpp
using namespace AscendC::Te;
constexpr uint64_t gmAddr = 128;

// Example 1: Slice by Shape.
auto layout1 = MakeLayout(MakeShape(10, 20), MakeStride(20, 1));
auto tensor1 = MakeTensor(MakeMemPtr<Location::GM, float>(gmAddr), layout1);
auto sub1 = Slice(tensor1, MakeCoord(2, 4), MakeShape(4, 8));
// The start coordinate is (2, 4), and the sub-tensor shape is (4, 8).

// Example 2: Slice by Layout.
auto layout2 = MakeLayout(MakeShape(10, 20), MakeStride(20, 1));
auto tensor2 = MakeTensor(MakeMemPtr<Location::GM, float>(gmAddr), layout2);
auto sub2 = Slice(tensor2, MakeCoord(1, 2), MakeLayout(MakeShape(3, 5), MakeStride(20, 1)));
// Slice a sub-tensor with Shape (3, 5) starting from (1, 2), and keep the Stride structure.

// Example 3: Equivalent to calling the member function.
auto sub3 = tensor2.Slice(MakeCoord(1, 2), MakeShape(3, 5));
// sub3 is equivalent to Slice(tensor2, MakeCoord(1, 2), MakeShape(3, 5)).
```

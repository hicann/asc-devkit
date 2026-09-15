# Capacity

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:45:39.885Z -->

> [!NOTE] Note
> This API is experimental and may be adjusted or improved in later versions. Backward compatibility is not guaranteed. Developers are advised to keep track of updates in later versions.

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

**Capacity** is used to calculate the actual memory capacity required by a **Layout**.

Unlike **Size** (the number of valid elements), **Capacity** includes the invalid data regions caused by non-contiguous strides. For contiguous layouts (compact strides), **Capacity** equals **Size**; for layouts with gaps, **Capacity** is usually greater than **Size**.

## Function Prototype

  ```cpp
  template <size_t... Is, typename Shape, typename Stride>
  __aicore__ inline constexpr auto Capacity(const Layout<Shape, Stride>& layout)
  ```

## Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| **Shape** | Input | Type of the shape that constitutes the **Layout**, that is, a tuple type. |
| **Stride** | Input | Type of the stride that constitutes the **Layout**, that is, a tuple type. |
| **Is...** | size_t | Index sequence that specifies the sub-dimension range. The capacity is calculated only for the selected dimensions. |

**Table 2** Parameter description

  | Parameter | Type | Description |
|--------|------|------|
| **layout** | Input | Layout used to describe the tensor. |

## Return Value

Returns the actual memory size occupied by **Layout**, including valid data and dirty data.

## Constraints

**Is...** must be an index within the valid range.

## Example

```cpp
using namespace AscendC::Te;

// Example 1: Compact layout, Capacity = Size.
auto layout1 = MakeLayout(MakeShape(10, 20), MakeStride(20, 1));
auto cap1 = Capacity(layout1);
// dim0: 10 * 20 = 200; dim1: 20 * 1 = 20; max = 200
// Size = 10 * 20 = 200

// Example 2: Non-contiguous layout, Capacity > Size.
auto layout2 = MakeLayout(MakeShape(10, 20), MakeStride(1, 100));
auto cap2 = Capacity(layout2);
// dim0: 10 * 1 = 10; dim1: 20 * 100 = 2000; max = 2000
// Size = 10 * 20 = 200

// Example 3: Specify a sub-dimension.
auto layout3 = MakeLayout(MakeShape(10, 20, 30), MakeStride(1, 10, 200));
auto cap3All = Capacity(layout3);       // max(10*1, 20*10, 30*200) = max(10, 200, 6000) = 6000
auto cap3_0_1 = Capacity<0, 1>(layout3); // max(10*1, 20*10) = max(10, 200) = 200
```

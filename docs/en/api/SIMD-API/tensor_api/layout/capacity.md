# capacity

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T02:23:11.579Z pushedAt=2026-10-27T13:03:35.295Z nl=2 -->

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

capacity is used to calculate the actual memory capacity required by a layout.

Unlike size (the number of valid elements), capacity includes invalid data regions caused by non-contiguous strides. For contiguous arrangements (compact strides), capacity equals size; for arrangements with gaps, capacity is usually greater than size.

## Function Prototype

  ```cpp
  template <size_t... Is, typename Layout>
  __aicore__ inline constexpr auto capacity(const Layout& layout)
  ```

## Parameters

**Table 1** Template parameter description

  | Parameter | Type | Description |
|--------|------|------|
| Is... | size_t | Index sequence that specifies the sub-dimension range. The capacity is calculated only for the selected dimensions. |
| Layout | Input | Type of the layout object. |

**Table 2**  Parameters

  | Parameter | Type | Description |
|--------|------|------|
| layout | Input | Layout used to describe the tensor. |

## Return Value

Returns the actual area occupied by the layout, including valid data and dirty data.

## Constraints

Is... must be an index within the valid range.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// Example 1: Compact layout, capacity = size.
auto layout1 = make_layout(make_shape(10, 20), make_stride(20, 1));
auto cap1 = capacity(layout1);
// dim0: 10 * 20 = 200; dim1: 20 * 1 = 20; max = 200
// size = 10 * 20 = 200

// Example 2: Non-contiguous layout, capacity > size.
auto layout2 = make_layout(make_shape(10, 20), make_stride(1, 100));
auto cap2 = capacity(layout2);
// dim0: 10 * 1 = 10; dim1: 20 * 100 = 2000; max = 2000
// size = 10 * 20 = 200

// Example 3: Specify a sub-dimension.
auto layout3 = make_layout(make_shape(10, 20, 30), make_stride(1, 10, 200));
auto cap3_all = capacity(layout3);       // max(10*1, 20*10, 30*200) = max(10, 200, 6000) = 6000
auto cap3_0_1 = capacity<0, 1>(layout3); // max(10*1, 20*10) = max(10, 200) = 200
```

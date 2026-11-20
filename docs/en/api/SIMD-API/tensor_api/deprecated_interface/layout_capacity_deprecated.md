# layout::Capacity (Deprecated)

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T02:13:55.802Z pushedAt=2026-10-27T13:03:35.275Z nl=2 -->

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

**This API is deprecated. Use [layout::capacity](../layout/layout.md#capacity) instead.**

Returns the actual memory capacity of the current Layout or the layout corresponding to the specified sub-dimension, including invalid data regions caused by non-contiguous Stride.

## Function Prototype

```cpp
template <size_t... I>
__aicore__ inline constexpr decltype(auto) Capacity() const;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| I... | Input | Template parameter, indicating the sequence of sub-dimension indexes to be accessed. When not specified, the complete Layout is processed. |

## Return Value

Returns the actual capacity of the current Layout or the layout corresponding to the specified sub-dimensions.

## Pipeline Type

None

## Constraints

`I...` must be a valid index in Layout.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto old_capacity = layout.Capacity();      // Deprecated API.
auto new_capacity = layout.capacity();       // Substitute API.
```

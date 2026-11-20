# base_tensor::Layout (Deprecated)

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T02:19:54.885Z pushedAt=2026-10-27T13:03:35.287Z nl=2 -->

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

**This API is deprecated. Use [base_tensor::layout](../tensor/tensor.md#layout) instead.**

Obtains the Layout object bound to the Tensor.

## Function Prototype

```cpp
__aicore__ inline constexpr decltype(auto) Layout() const;
```

## Parameters

None

## Return Value

Returns a constant reference to the Layout object.

## Pipeline Type

None

## Constraints

None

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

const auto& old_layout = tensor.Layout();    // Deprecated API.
const auto& new_layout = tensor.layout();    // Substitute API.
```

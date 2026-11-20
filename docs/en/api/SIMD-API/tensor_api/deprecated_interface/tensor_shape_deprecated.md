# base_tensor::Shape (Deprecated)

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T02:19:55.023Z pushedAt=2026-10-27T13:03:35.289Z nl=2 -->

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

**This API is deprecated. Use [base_tensor::shape](../tensor/tensor.md#shape) instead.**

Obtains the shape information in the layout bound to the tensor.

## Function Prototype

```cpp
__aicore__ inline constexpr decltype(auto) Shape() const;
```

## Parameters

None

## Return Value

Returns the result of `layout().shape()`.

## Pipeline Type

None

## Constraints

None

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

const auto& old_shape = tensor.Shape();      // Deprecated API.
const auto& new_shape = tensor.shape();      // Recommended replacement.
```

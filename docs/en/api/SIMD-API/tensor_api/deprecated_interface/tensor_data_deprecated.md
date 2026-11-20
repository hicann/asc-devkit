# base_tensor::Data (Deprecated)

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T02:18:45.473Z pushedAt=2026-10-27T13:03:35.284Z nl=2 -->

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

**This API is deprecated. Use [base_tensor::data](../tensor/tensor.md#data) instead.**

Obtains the begin iterator of the underlying data of a Tensor.

## Function Prototype

```cpp
__aicore__ inline constexpr decltype(auto) Data() const;
__aicore__ inline constexpr decltype(auto) Data();
```

## Parameters

None

## Return Value

Returns the result of `engine().begin()`; the const overload returns the corresponding constant access result.

## Pipeline Type

None

## Constraints

The Engine bound to the Tensor must be valid.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto old_data = tensor.Data();               // Deprecated API.
auto new_data = tensor.data();               // Substitute API.
```

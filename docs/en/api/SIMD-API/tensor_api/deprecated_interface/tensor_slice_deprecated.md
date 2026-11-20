# base_tensor::Slice (Deprecated)

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T02:21:06.080Z pushedAt=2026-10-27T13:03:35.291Z nl=2 -->

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

**This API is deprecated. Use [base_tensor::slice](../tensor/tensor.md#slice) instead.**

Cuts out a sub-tensor with new Layout information from the specified coordinates.

## Function Prototype

```cpp
template <typename Coord, typename Info>
__aicore__ inline decltype(auto) Slice(const Coord& coord, const Info& info);

template <typename Coord, typename Info>
__aicore__ inline decltype(auto) Slice(const Coord& coord, const Info& info) const;
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| Coord | Input | Template parameter, indicating the type of the slice start coordinate. |
| Info | Input | Template parameter, indicating the slice description type, which can be Shape or Layout. |
| coord | Input | Start coordinate of the sub-tensor in the current tensor. |
| info | Input | Shape or Layout description of the sub-tensor. |

## Return Value

Returns the sliced sub-tensor. The Engine of the new tensor points to the slice start position, and the Layout is generated based on `coord` and `info`.

## Pipeline Type

None

## Constraints

The structures and values of `coord` and `info` must comply with the constraints of the [slice](../tensor/slice.md) API.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto old_slice = tensor.Slice(coord, info);  // Deprecated API.
auto new_slice = tensor.slice(coord, info);  // Substitute API.
```

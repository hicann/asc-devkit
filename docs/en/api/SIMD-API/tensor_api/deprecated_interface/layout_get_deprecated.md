# layout::Get (Deprecated)

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T02:13:56.825Z pushedAt=2026-10-27T13:03:35.276Z nl=2 -->

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

**This API is deprecated. Use [layout::get](../layout/layout.md#get) instead.**

Extracts elements or substructures from Shape and Stride in Layout according to the template index.

## Function Prototype

```cpp
template <size_t... I>
__aicore__ inline constexpr decltype(auto) Get();

template <size_t... I>
__aicore__ inline constexpr decltype(auto) Get() const;
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| I... | Input | Template parameter. The first index 0 indicates Shape, and 1 indicates Stride. Subsequent indexes are used to further access nested substructures. |

## Return Value

Returns a reference to the element or substructure at the specified position; the const overload returns a constant reference.

## Pipeline Type

None

## Constraints

`I...` must be a valid index in Layout and its nested tuples.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto& old_value = layout.Get<0, 1>();       // Deprecated API.
auto& new_value = layout.get<0, 1>();       // Substitute API.
```

# Dimension Retention Mark

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-10-27T04:26:19.304Z pushedAt=2026-10-27T13:03:35.493Z nl=2 -->

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

Tensor API provides the `keep_mark` type and the constant `_`, which are currently used only to mark the dimensions to be retained in the `squeeze` API.

## Prototype

```cpp
struct keep_mark {
    __aicore__ inline constexpr keep_mark() = default;
};

inline constexpr keep_mark _{};
```

## Types

**Table 1**  Types

| Type or Constant | Description |
| :--- | :--- |
| `keep_mark` | Dimension retention mark type. |
| `_` | Global compile-time constant of the `keep_mark` type, used to indicate that the corresponding dimension is retained. |

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// Delete the second dimension of size 1, keeping the first and third dimensions.
auto source_layout = make_layout(make_shape(4, _1{}, 8));
auto squeezed_layout = squeeze(source_layout, make_coord(_, _1{}, _));
```

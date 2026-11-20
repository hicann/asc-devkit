# get_shape

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T02:26:05.666Z pushedAt=2026-10-27T13:03:35.300Z nl=2 -->

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

`get_shape` is used to extract the shape part from the [layout](./layout.md):

- **Without specifying the template parameter Is...**: Returns the complete shape tuple.
- **With the template parameter Is... specified**: Selects the element at the corresponding index from the top-level tuple of the shape. When a single index is specified, that element is returned; when multiple indexes are specified, a new tuple composed of these top-level elements is returned.

## Function Prototype

```cpp
template <size_t... Is, typename Layout>
__aicore__ inline constexpr auto get_shape(const Layout& layout)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| -------- | ----------- | ------ |
| Is... | Input | Optional. Used to select one or more indexes in the top-level tuple of the shape. |
| Layout | Input | Type of the layout object. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| -------- | ----------- | ------ |
| layout | Input | layout object. |

## Return Value

Returns the shape object (or its substructure) that describes the tensor shape.

## Constraints

Is... must be within the valid range of the top-level tuple of the shape.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto layout = make_layout(make_shape(10, 20, 30));

// If Is... is not specified, the full shape tuple is returned.
auto full_shape = get_shape(layout);           // (10, 20, 30)

// If a single index is specified, the corresponding element is returned.
auto s0 = get_shape<0>(layout);              // 10

// If multiple indexes are specified, a new tuple is formed.
auto s01 = get_shape<0, 1>(layout);          // (10, 20)
```

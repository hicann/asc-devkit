# get_stride

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T02:26:41.234Z pushedAt=2026-10-27T13:03:35.301Z nl=2 -->

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

get_stride is used to extract the stride part from a layout:

- **Without specifying the template parameter Is...**: Returns the complete stride tuple.
- **With the template parameter Is... specified**: Selects the elements at the corresponding indexes from the top-level stride tuple. When a single index is specified, that element is returned; when multiple indexes are specified, a new tuple composed of these top-level elements is returned.

## Function Prototype

```cpp
template <size_t... Is, typename Layout>
__aicore__ inline constexpr auto get_stride(const Layout& layout)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| -------- | ----------- | ------ |
| Is... | Input | Optional. Used to select one or more indexes in the top-level stride tuple. |
| Layout | Input | Type of the layout object. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| -------- | ----------- | ------ |
| layout | Input | layout object. |

## Return Value

Returns the stride object (or its substructure) that describes the memory access stride. The stride unit is the number of elements. For details, see [stride](./stride.md).

## Constraints

Is... must be within the valid range of the top-level stride tuple.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto layout = make_layout(make_shape(10, 20, 30), make_stride(1, 10, 200));

// When Is... is not specified -> return the complete stride tuple.
auto full_stride = get_stride(layout);         // (1, 10, 200)

// Specify a single index -> return the corresponding element.
auto s0 = get_stride<0>(layout);             // 1

// Specify multiple indexes -> form a new tuple.
auto s01 = get_stride<0, 1>(layout);         // (1, 10)
```

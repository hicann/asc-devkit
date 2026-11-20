# make_shape

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T02:34:20.938Z pushedAt=2026-10-27T13:03:35.309Z nl=2 -->

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

Constructs a shape object to describe the logical shape of data. It supports passing multiple dimension values and also supports passing nested shape substructures to construct a hierarchical shape.

## Function Prototype

```cpp
template <typename FirstShape, typename... Shapes>
__aicore__ inline constexpr shape<FirstShape, Shapes...> make_shape(
    const FirstShape& first_shape, const Shapes&... shapes)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| FirstShape | Input | Type of the first shape element, which can be an integer variable, an Std::Int integer constant, or a nested shape substructure type. |
| Shapes... | Input | Types of the remaining shape elements. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
|--------|-----------|------|
| first_shape | Input | Value or substructure of the first dimension. |
| shapes... | Input | Values or substructures of the remaining dimensions. |

## Return Value

Returns a `shape<FirstShape, Shapes...>` object.

## Constraints

- The input parameters cannot be empty.
- The input parameters are positive integers.
- Supported data types include integer types such as size_t and int, or the Std::Int type.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// Construct a regular shape.
auto shape = make_shape(10, 20, 30);

// Construct a hierarchical shape.
auto fractal_shape = make_shape(make_shape(16, 8), make_shape(32, 4));

auto dim0 = Std::get<0>(shape);                // dim0 = 10
auto inner_row = Std::get<0>(Std::get<0>(fractal_shape)); // inner_row = 16
```

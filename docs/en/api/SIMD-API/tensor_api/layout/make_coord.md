# make_coord

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T02:30:03.953Z pushedAt=2026-10-27T13:03:35.305Z nl=2 -->

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

Constructs a coord object to define the coordinates of a tensor.

## Function Prototype

```cpp
template <typename FirstCoord, typename... Coords>
__aicore__ inline constexpr coord<FirstCoord, Coords...> make_coord(
    const FirstCoord& first_coord, const Coords&... coords)
```

## Parameters

**Table 1** Template parameter type description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| FirstCoord | Input | Data type of the first coordinate. Supports integer types such as size_t and int, as well as the Std::Int type. |
| Coords... | Input | Data types of the remaining coordinates. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
|--------|-----------|------|
| first_coord | Input | Coordinate of the first dimension. |
| coords... | Input | Coordinates of the remaining dimensions. |

## Return Value

Returns a `coord<FirstCoord, Coords...>` object.

## Constraints

- The coordinate value of each dimension must be a non-negative integer.
- The number of parameters must match the number of dimensions of the corresponding shape.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// Create the coordinates of a 3D tensor using an integer type.
auto coord = asc::te::make_coord(5, 10, 15);

// Obtain the coordinates of each dimension.
auto coord0 = AscendC::Std::get<0>(coord); // coord0 = 5
auto coord1 = AscendC::Std::get<1>(coord); // coord1 = 10
auto coord2 = AscendC::Std::get<2>(coord); // coord2 = 15

// Create the coordinates of a 3D tensor using the Std::Int type.
auto coord_int = asc::te::make_coord(
    AscendC::Std::Int<5>{},
    AscendC::Std::Int<10>{},
    AscendC::Std::Int<15>{});

// Obtain the coordinates of each dimension.
auto coord_int0 = AscendC::Std::get<0>(coord_int); // coord_int0 = 5
auto coord_int1 = AscendC::Std::get<1>(coord_int); // coord_int1 = 10
auto coord_int2 = AscendC::Std::get<2>(coord_int); // coord_int2 = 15
```

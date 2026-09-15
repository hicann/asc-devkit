# MakeCoord

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:51:23.162Z -->

> [!NOTE]NOTE
> This API is experimental and may be adjusted or improved in later versions. Backward compatibility is not guaranteed. Developers should pay attention to updates in later versions when using this API.

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file to include is `#include "tensor_api/tensor.h"`.

Constructs a **Coord** object to define the coordinates of a tensor.

## Prototype

```cpp
template <typename... Ts>
__aicore__ inline constexpr Coord<Ts...> MakeCoord(const Ts&... t)
```

## Parameters

**Table 1** Template parameter type description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| **Ts...** | Input | Variadic template parameters that specify the data type of each dimension coordinate. Integer types such as size_t and int, as well as the **Std::Int** type, are supported. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| **t** | Input | Coordinate of each dimension, as a variadic parameter. |

## Return Value

Returns a **Coord<Ts...>** object.

## Constraints

- The coordinate value of each dimension must be a non-negative integer.
- The number of parameters must be consistent with the number of dimensions of the corresponding **Shape**.

## Example

```cpp
// Create the coordinates of a 3D tensor using integer types.
auto coord = AscendC::Te::MakeCoord(5, 10, 15);

// Obtain the coordinates of each dimension.
auto coord0 = AscendC::Std::get<0>(coord); // coord0 = 5
auto coord1 = AscendC::Std::get<1>(coord); // coord1 = 10
auto coord2 = AscendC::Std::get<2>(coord); // coord2 = 15

// Create the coordinates of a 3D tensor using the Std::Int type.
auto coordInt = AscendC::Te::MakeCoord(
    AscendC::Std::Int<5>{},
    AscendC::Std::Int<10>{},
    AscendC::Std::Int<15>{});

// Obtain the coordinates of each dimension.
auto coordInt0 = AscendC::Std::get<0>(coordInt); // coordInt0 = 5
auto coordInt1 = AscendC::Std::get<1>(coordInt); // coordInt1 = 10
auto coordInt2 = AscendC::Std::get<2>(coordInt); // coordInt2 = 15
```

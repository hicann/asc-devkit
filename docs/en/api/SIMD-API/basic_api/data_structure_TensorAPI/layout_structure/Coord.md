# Coord

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:58:38.908Z -->

> [!NOTE]NOTE
> This is an experimental API that may be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers are advised to keep track of updates in later versions.

**Coord** is used to define the coordinates of a tensor for accessing elements at specific positions in the tensor.

## Header File to Include

```cpp
#include "tensor_api/tensor.h"
```

## Prototype

```cpp
template <typename... Coords>
using Coord = Std::tuple<Coords...>;
```

## Parameters

| Parameter | Input/Output | Description |
|--------|----------|------|
| **Coords...** | Input | Variadic template representing the coordinates of each dimension. |

## API Mapping

Coord is created through the [MakeCoord](MakeCoord.md) function, can be converted from multi-dimensional coordinates to linear coordinates through the [Crd2Idx](../../aux_data_structures/Coordinate/Crd2Idx.md) function, and can be passed as a parameter to the [operator()](Layout.md) member function of **Layout**, the [Slice](../tensor_structure/Tensor.md) member function of **Tensor**, and the [Slice](../tensor_structure/Slice.md) API to perform tensor offset and slicing operations.

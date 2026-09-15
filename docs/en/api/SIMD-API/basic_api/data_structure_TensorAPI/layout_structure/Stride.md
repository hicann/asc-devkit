# Stride

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T18:01:48.204Z -->

> [!NOTE]Note
> This is an experimental API. It may be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers are advised to keep track of updates in later versions.

**Stride** defines the stride of each dimension of a tensor in memory, describing the interval between adjacent elements in the same dimension in memory.

## Header File to Include

```cpp
#include "tensor_api/tensor.h"
```

## Prototype

```cpp
template <typename... Strides>
using Stride = Std::tuple<Strides...>;
```

## Parameters

| Parameter | Input/Output | Description |
|--------|----------|------|
| **Strides...** | Input | Variadic template representing the stride value of each dimension, which can also represent nested Stride substructures. |

## API Mapping

**Stride** is usually created through the [MakeStride](MakeStride.md) function, and obtained through the [GetStride](GetStride.md) function and the [Stride](Layout.md) member function of **Layout**.

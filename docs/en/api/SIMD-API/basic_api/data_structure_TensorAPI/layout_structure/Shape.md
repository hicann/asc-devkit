# Shape

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T18:00:07.563Z -->

> [!NOTE]
> This is an experimental API and may be adjusted or improved in later versions. No backward compatibility is guaranteed. Developers are advised to keep track of updates in later versions.

**Shape** defines the logical shape of a tensor, describing the size of each dimension.

## Header Files

```cpp
#include "tensor_api/tensor.h"
```

## Prototype

```cpp
template <typename... Shapes>
using Shape = Std::tuple<Shapes...>;
```

## Parameters

| Parameter | Input/Output | Description |
|--------|----------|------|
| Shapes... | Input | Variadic template representing the shape value of each dimension, or a nested Shape substructure. |

## API Mapping

**Shape** is created through the [MakeShape](MakeShape.md) function and obtained through the [GetShape](GetShape.md) function and the **Shape** member of **Layout**.

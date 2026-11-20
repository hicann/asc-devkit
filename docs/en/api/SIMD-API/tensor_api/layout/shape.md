# shape

<!-- md-trans-meta sourceCommit=46a50d69bd092786e44495d519df57743eefbfc6 translatedAt=2026-10-27T02:37:26.598Z pushedAt=2026-10-27T13:03:35.317Z nl=2 -->

shape is used to define the logical shape of a tensor, describing the size of each dimension.

The header file path is `tensor_api/tensor.h`.

## Prototype

```cpp
template <typename... Shapes>
using shape = Std::tuple<Shapes...>;
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
|--------|----------|------|
| Shapes... | Input | Variadic template representing the shape value of each dimension, which can also represent nested shape substructures. |

## API Mapping

shape is created by the [make_shape](make_shape.md) function and obtained by the [get_shape](get_shape.md) function and the [shape](layout.md) member of layout.

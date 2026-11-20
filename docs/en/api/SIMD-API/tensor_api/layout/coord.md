# coord

<!-- md-trans-meta sourceCommit=46a50d69bd092786e44495d519df57743eefbfc6 translatedAt=2026-10-27T02:23:16.811Z pushedAt=2026-10-27T13:03:35.293Z nl=2 -->

coord is used to define the coordinates of a tensor for accessing elements at specific positions in the tensor.

The header file path is `tensor_api/tensor.h`.

## Prototype

```cpp
template <typename... Coords>
using coord = Std::tuple<Coords...>;
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
|--------|----------|------|
| Coords... | Input | Variadic template representing the coordinates of each dimension.|

## API Mapping

coord is created through the [make_coord](make_coord.md) function. It can be converted from multidimensional coordinates to linear coordinates through the [crd2idx](crd2idx.md) function and can be passed as a parameter to the [operator()](layout.md) member function of layout, the [slice](../tensor/tensor.md) member function of Tensor, and the [slice](../tensor/slice.md) API to perform tensor offset and slicing operations.

# layout

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T02:29:09.912Z pushedAt=2026-10-27T13:03:35.306Z nl=2 -->

The `layout<shape, stride>` data structure is a fundamental template class for describing the memory layout of multidimensional tensors. Through compile-time shape and stride information, it implements the mapping from the logical coordinate space to the one-dimensional memory address space. By leveraging template metaprogramming techniques, this class performs computation and code generation at compile time, thereby reducing runtime overhead.

layout contains two core components:

- **shape**: Defines the logical shape of the data, such as the number of rows and columns of a two-dimensional cube or the size of each dimension of a multidimensional tensor.
- **stride**: Defines the stride of each dimension in memory, that is, the interval in memory between adjacent elements of the same dimension, measured in elements, and corresponds one-to-one with the dimension information of shape.

For example, a two-dimensional cube with a shape of (4, 2) and a stride of (4, 1) means:

- The cube has 4 rows and 2 columns.
- The column-direction stride is 1, meaning that adjacent elements in each row are separated by 1 element; the row-direction stride is 4, meaning that the starting addresses of adjacent rows are separated by 4 elements.

**Table 1**  Linear address view

| Address | 0 | 1 | 2 | 4 | 5 | 6 | 8 | 9 | 10 | 12 | 13 |
|------|---|---|---|---|---|---|---|---|----|----|----|
| Element | a00 | a01 | - | a10 | a11 | - | a20 | a21 | - | a30 | a31 |

**Table 2** Cube logical view

| Index | Column 0 | Column 1 |
|------|------|------|
| Row 0 | a00 (address 0) | a01 (address 1) |
| Row 1 | a10 (address 4) | a11 (address 5) |
| Row 2 | a20 (address 8) | a21 (address 9) |
| Row 3 | a30 (address 12) | a31 (address 13) |

The header file path is `tensor_api/tensor.h`.

## Prototype

```cpp
template <typename ShapeTuple, typename StrideTuple, typename Info = Std::ignore_t>
struct layout : private Std::tuple<ShapeTuple, StrideTuple>
{
public:
    static constexpr auto depth = nesting_depth_v<ShapeTuple>;
    static constexpr auto rank_size = Std::tuple_size_v<ShapeTuple>;

    __aicore__ inline constexpr layout(
        const ShapeTuple& shape = {}, const StrideTuple& stride = {});

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) capacity() const

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) shape()
    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) shape() const

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) stride()
    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) stride() const

    template <typename Coord>
    __aicore__ inline constexpr auto operator()(const Coord& coord) const;

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) rank() const

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) size() const

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) get()
    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) get() const

private:
    using tag = Info;
};
```

## Template Parameters

**Table 3** Template parameter description

| Parameter | Description |
|--------|------|
| ShapeTuple | Std::tuple structure type used to define the logical shape of the data. |
| StrideTuple | Std::tuple structure type used to define the stride of each dimension in memory, corresponding one-to-one with the dimension information of shape. |
| Info | Optional template parameter, defaulting to Std::ignore_t. When layout is constructed by make_frame_layout or related APIs, this parameter can carry additional information such as the layout mode and layout features. |

## Member Functions

### layout()

- Description

    Constructs a layout object and initializes the layout information with the given shape and stride.

- Function Prototype

```cpp
  __aicore__ inline constexpr layout(
      const ShapeTuple& shape = {}, const StrideTuple& stride = {})
```

- Parameters

**Table 4**  Parameter description of layout()

  | Parameter | Input/Output | Description |
  | :----- | :------- | :------- |
  | shape | Input | Shape information of the tensor, which must be of the Std::tuple type. The unit is the number of elements. |
  | stride | Input | Stride information of the tensor, which must be of the Std::tuple type. The unit is the number of elements. |

- Return Value

    Constructor with no return value.

---

### capacity()

- Description

    Obtains the actual memory capacity required by the current Layout.

    When template parameters I... are passed in, the specified sub-dimensions are selected first, and then the capacity of the corresponding sub-layout is calculated.

- Function Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) capacity() const
  ```

- Return Value

    Returns the actual capacity of the current layout or the layout corresponding to the specified sub-dimensions.

---

### shape()

- Description

    Obtains the shape information in the layout.

    When the template parameter I... is not specified, returns the complete shape; when I... is specified, returns the substructure corresponding to the specified sub-dimension.

- Function Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) shape()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) shape() const
  ```

- Return Value

    Returns a (constant) reference to the shape object or its substructure.

---

### stride()

- Description

    Obtains the stride information in layout.

    When the template parameter I... is not specified, returns the complete stride; when I... is passed, returns the substructure corresponding to the specified sub-dimension.

- Function Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) stride()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) stride() const
  ```

- Return Value

    Returns a (constant) reference to the stride object or its substructure.

---

### operator()

- Description

    Maps multidimensional logical coordinates to a one-dimensional linear index.

    This API internally calls [crd2idx](crd2idx.md) to compute the linear position of the coordinates in memory based on the shape and stride of the current layout.

- Function Prototype

  ```cpp
  template <typename Coord>
  __aicore__ inline constexpr auto operator()(const Coord& coord) const
  ```

- Parameters

**Table 5**  operator() parameter description

  | Parameter | Input/Output | Description |
  | :----- | :------- | :------- |
  | coord | Input | Multidimensional logical coordinates. |

- Return Value

    Returns the one-dimensional linear index corresponding to the coordinate coord.

---

### rank()

- Description

    Obtains the rank of the layout, that is, the number of dimensions.

    When the template parameter I... is not specified, returns the rank of the complete layout; when I... is specified, returns the rank of the specified sub-dimension.

- Function Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) rank() const
  ```

- Return Value

    Returns the rank of the current Layout or of a specified sub-dimension.

---

### size()

- Description

    Obtains the total number of elements covered by layout.

    When the template parameter I... is not specified, returns the total number of elements corresponding to the complete shape; when I... is passed in, returns the total number of elements under the specified sub-dimension.

- Function Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) size() const
  ```

- Return Value

    Returns the total number of elements covered by the current layout or the specified sub-dimension.

---

### get()

- Description

    Extracts the element or substructure at the specified position from layout.

    Position 0 corresponds to shape, and position 1 corresponds to stride; with the template parameter I..., elements in the sub-tuple can be further extracted.

- Function Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) get()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) get() const
  ```

- Return Value

    Returns a (constant) reference to the element or substructure at the specified position.

---

### depth

- Description

    A compile-time constant that indicates the nesting depth of the shape type, used to determine whether the current layout is a nested layout structure.

---

### rank

- Description

    Compile-time constant that indicates the rank of the complete layout.

## `layout` and Member Function Example

The following uses a two-dimensional `layout` example to illustrate how the `layout` object itself and each member function are used and what they return.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto layout_obj = layout(make_shape(4, 2), make_stride(4, 1));

auto shape = layout_obj.shape();
// shape = (4, 2)

auto shape0 = layout_obj.shape<0>();
// shape0 = 4

auto stride = layout_obj.stride();
// stride = (4, 1)

auto stride1 = layout_obj.stride<1>();
// stride1 = 1

auto rank = layout_obj.rank();
// rank = 2

auto size = layout_obj.size();
// size = 8

auto capacity = layout_obj.capacity();
// capacity = 16

auto index = layout_obj(make_coord(1, 1));
// index = 5

auto shape_info = layout_obj.get<0>();
// shape_info = (4, 2)

auto stride_info = layout_obj.get<1>();
// stride_info = (4, 1)

constexpr auto depth = decltype(layout_obj)::depth;
// depth = 2

constexpr auto full_rank = decltype(layout_obj)::rank_size;
// full_rank = 2
```

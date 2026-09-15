# Layout

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:52:15.450Z -->

> [!NOTE] Note
> This API is experimental and may be adjusted or improved in later versions. Backward compatibility is not guaranteed. Developers are advised to keep track of updates in subsequent versions.

The **Layout**<Shape, Stride> data structure is a fundamental template class for describing the memory layout of a multidimensional tensor. Through the compile-time shape **Shape** and stride **Stride** information, it implements the mapping from the logical coordinate space to the one-dimensional memory address space. Leveraging template metaprogramming techniques, this class performs computation and code generation at compile time, thereby reducing runtime overhead.

**Layout** consists of two core components:

- **Shape**: Defines the logical shape of the data, such as the number of rows and columns of a two-dimensional cube or the size of each dimension of a multidimensional tensor.
- **Stride**: Defines the stride of each dimension in memory, that is, the interval between adjacent elements of the same dimension in memory, measured in elements, and corresponds one-to-one with the dimension information of **Shape**.

For example, a two-dimensional cube with **Shape** (4, 2) and **Stride** (4, 1) means:

- The cube has 4 rows and 2 columns.
- The column-direction stride is 1, meaning adjacent elements in each row are separated by 1 element; the row-direction stride is 4, meaning the starting addresses of adjacent rows are separated by 4 elements.

**Table 1** Linear address view

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

## Header Files to Include

```cpp
#include "tensor_api/tensor.h"
```

## Prototype Definition

```cpp
template <typename T, typename U, typename Info = Std::ignore_t>
struct Layout : private Std::tuple<T, U>
{
    static constexpr auto depth = NestingDepthV<T>;
    static constexpr auto rank = Std::tuple_size_v<T>;

    __aicore__ inline constexpr Layout(const T& shape  = {}, const U& stride = {})
        : Std::tuple<T, U>(shape, stride)

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Capacity() const

    __aicore__ inline constexpr decltype(auto) layout()
    __aicore__ inline constexpr decltype(auto) layout() const

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Shape()
    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Shape() const

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Stride()
    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Stride() const

    template <typename S>
    __aicore__ inline constexpr auto operator()(const S& coord) const

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Rank() const

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Size() const

    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Get()
    template <size_t... I>
    __aicore__ inline constexpr decltype(auto) Get() const
};
```

## Template Parameters

| Parameter | Description |
|--------|------|
| T | **Std::tuple** structure type used to define the logical shape of the data. |
| U | **Std::tuple** structure type used to define the stride of each dimension in memory, corresponding one-to-one with the dimension information of **Shape**. |
| Info | Optional template parameter, defaulting to **Std::ignore_t**. When **Layout** is constructed by **MakeFrameLayout** or related APIs, this parameter can carry additional information such as the layout mode and layout features. |

## Member Functions

### Layout()

- Description

    Constructs a **Layout** object and initializes the layout information using the given **Shape** and **Stride**.

- Function Prototype

```cpp
  __aicore__ inline constexpr Layout(const T& shape = {}, const U& stride = {})
```

- Parameters

  | Parameter Name | Input/Output | Description |
  | :----- | :------- | :------- |
  | shape | Input | Shape information of the tensor, which must be of the **Std::tuple** type. |
  | stride | Input | Stride information of the tensor, which must be of the **Std::tuple** type. |

- Return Value Description

    Constructor, which has no return value.



### Capacity()

- Description

    Obtains the actual memory capacity required by the current Layout.

    When template parameters I... are passed in, the specified child dimensions are selected first, and then the capacity of the corresponding child layout is calculated.

- Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Capacity() const
  ```

- Return Value

    Returns the actual capacity of the current Layout or the layout corresponding to the specified child dimensions.



### layout()

- Description

    Obtains the **Layout** object itself, used to return a **Layout** layout instance in a unified API.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) layout()
  __aicore__ inline constexpr decltype(auto) layout() const
  ```

- Return Value Description

    Returns a (constant) reference to the current **Layout** object.



### Shape()

- Description

    Obtains the Shape information in the Layout.

    When no template parameter I... is specified, the complete Shape is returned; when I... is passed in, the child structure corresponding to the specified child dimension is returned.

- Function Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Shape()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Shape() const
  ```

- Return Value Description

    Returns the (constant) reference to the Shape object or its child structure.



### Stride()

- Description

    Obtains the **Stride** information in **Layout**.

    When the template parameter **I...** is not specified, returns the complete **Stride**; when **I...** is passed in, returns the child structure corresponding to the specified child dimension.

- Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Stride()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Stride() const
  ```

- Return Value

    Returns a (constant) reference to the **Stride** object or its child structure.



### operator()

- Description

    Maps multidimensional logical coordinates to a one-dimensional linear index.

    This API internally calls [Crd2Idx](../../aux_data_structures/Coordinate/Crd2Idx.md) and computes the linear position of the coordinates in memory based on the **Shape** and **Stride** of the current **Layout**.

- Function Prototype

  ```cpp
  template <typename S>
  __aicore__ inline constexpr auto operator()(const S& coord) const
  ```

- Parameters

  | Parameter | Input/Output | Description |
  | :----- | :------- | :------- |
  | **coord** | Input | Multidimensional logical coordinates. |

- Return Value Description

    Returns the one-dimensional linear index corresponding to the coordinate **coord**.



### Rank()

- Description

    Obtains the rank of **Layout**, that is, the number of dimensions.

    When the template parameter I... is not specified, returns the rank of the complete **Layout**; when I... is passed in, returns the rank of the specified child dimension.

- Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Rank() const
  ```

- Return Value

    Returns the rank of the current **Layout** or the specified child dimension.



### Size()

- Description

    Obtains the total number of elements covered by **Layout**.

    When the template parameter **I...** is not specified, returns the total number of elements corresponding to the complete **Shape**; when **I...** is passed in, returns the total number of elements under the specified child dimension.

- Function Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Size() const
  ```

- Return Value Description

    Returns the total number of elements covered by the current **Layout** or the specified child dimension.



### Get()

- Description

    Extracts the element or substructure at the specified position from **Layout**.

    Position 0 corresponds to **Shape**, and position 1 corresponds to **Stride**. Together with the template parameter **I...**, elements in the sub-tuple can be further extracted.

- Function Prototype

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Get()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Get() const
  ```

- Return Value Description

    Returns a (constant) reference to the element or substructure at the specified position.



### depth

- Description

    A compile-time constant that indicates the nesting depth of the **Shape** type, used to determine whether the current **Layout** is a nested layout structure.



### rank

- Description

Compile-time constant that represents the rank of the complete **Layout**.

## **Layout** and Member Function Example

The following uses a two-dimensional `Layout` example to illustrate how the `Layout` object itself and each member function are used and what they return.

```cpp
using namespace AscendC::Te;

auto layout = Layout(MakeShape(4, 2), MakeStride(4, 1));

auto self = layout.layout();
// self = Layout(MakeShape(4, 2), MakeStride(4, 1))

auto shape = layout.Shape();
// shape = (4, 2)

auto shape0 = layout.Shape<0>();
// shape0 = 4

auto stride = layout.Stride();
// stride = (4, 1)

auto stride1 = layout.Stride<1>();
// stride1 = 1

auto rank = layout.Rank();
// rank = 2

auto size = layout.Size();
// size = 8

auto capacity = layout.Capacity();
// capacity = 16

auto index = layout(MakeCoord(1, 1));
// index = 5

auto shapeInfo = layout.Get<0>();
// shapeInfo = (4, 2)

auto strideInfo = layout.Get<1>();
// strideInfo = (4, 1)

constexpr auto depth = decltype(layout)::depth;
// depth = 1

constexpr auto fullRank = decltype(layout)::rank;
// fullRank = 2
```

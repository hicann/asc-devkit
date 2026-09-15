# Tensor

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T18:06:37.937Z -->

> [!NOTE]
> This API is experimental and may be adjusted or improved in later versions. Backward compatibility is not guaranteed. Developers are advised to keep track of updates in subsequent versions.

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not Supported
<!-- end id7 -->

## Description

The header file to be included is: `#include "tensor_api/tensor.h"`.

**Tensor** binds the memory access engine **Engine** with the layout **Layout**. **Engine** describes the physical storage location and access method, while **Layout** describes the shape and layout of the data. **Tensor** is divided into **GlobalTensor** and **LocalTensor**. **GlobalTensor** stores global data in Global Memory (external storage), and **LocalTensor** stores data in Local Memory (internal storage) on the AI Core. The common APIs of the two are provided by **BaseTensor<TensorAttribute<EngineType, LayoutType>>**.

## Prototype

```cpp
template <typename EngineType, typename LayoutType>
struct TensorAttribute {};

template <typename T>
struct BaseTensor {};

template <typename EngineType, typename LayoutType>
struct BaseTensor<TensorAttribute<EngineType, LayoutType>> {
    using iterator = typename EngineType::iterator;
    using valueType = typename EngineType::valueType;
    using elementType = typename EngineType::elementType;
    using reference = typename EngineType::reference;

    using engineType  = EngineType;
    using layoutType  = LayoutType;

    static constexpr int rank = LayoutType::rank;

    __aicore__ inline BaseTensor();
    __aicore__ inline BaseTensor(const EngineType& engine, const LayoutType& layout);

    __aicore__ inline constexpr decltype(auto) Tensor() const;
    __aicore__ inline constexpr decltype(auto) Engine() const;
    __aicore__ inline constexpr decltype(auto) Engine();
    __aicore__ inline constexpr decltype(auto) Layout() const;
    __aicore__ inline constexpr decltype(auto) Data() const;
    __aicore__ inline constexpr decltype(auto) Data();
    __aicore__ inline constexpr decltype(auto) Shape() const;
    __aicore__ inline constexpr decltype(auto) Stride() const;
    __aicore__ inline constexpr auto Size() const;
    __aicore__ inline constexpr auto Capacity() const;

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord);

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord) const;

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord& coord);

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord& coord) const;

    template <typename Coord0, typename Coord1, typename... Coords>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs);

    template <typename Coord0, typename Coord1, typename... Coords>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs) const;

    template <typename Coord, typename Info>
    __aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info);

    template <typename Coord, typename Info>
    __aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info) const;
};

template <typename EngineType, typename LayoutType>
struct GlobalTensor<TensorAttribute<EngineType, LayoutType>>
    : public BaseTensor<TensorAttribute<EngineType, LayoutType>> {
    __aicore__ inline GlobalTensor() = default;
    __aicore__ inline constexpr void SetL2CacheHint(Te::CacheMode mode);
};

template <typename EngineType, typename LayoutType>
struct LocalTensor<TensorAttribute<EngineType, LayoutType>>
    : public BaseTensor<TensorAttribute<EngineType, LayoutType>> {
    __aicore__ inline LocalTensor() = default;
};
```

## Parameters

Template Parameters

| Parameter | Type | Description |
|--------|------|------|
| EngineType | Input | Memory access engine type of the tensor. |
| LayoutType | Input | Layout type of the tensor, used to describe **Shape** and **Stride**. |
| T | Input | Generic template parameter used for base template or specialization matching. |

## Return Value

Tensor-related types are type definitions and do not directly return a value.

When constructing a tensor through **MakeTensor**: if the data is global data stored in Global Memory (external storage), a **GlobalTensor** is returned; if the data is stored in Local Memory (internal storage) on the AI Core, a **LocalTensor** is returned.

## Member Functions

### Tensor()

- Description

    Get the tensor object itself.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) Tensor() const
  ```

- Return Value

    Returns a constant reference to the current **Tensor** object.



### Engine()

- Description

    Obtains the **Engine** object bound to the tensor.

- Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) Engine() const
  __aicore__ inline constexpr decltype(auto) Engine()
  ```

- Return Value

    Returns the (constant) reference to the **Engine** object.



### Layout()

- Description

    Obtains the **Layout** object bound to the tensor.

- Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) Layout() const
  ```

- Return Value

    Returns a constant reference to the **Layout** object.



### Data()

- Description

    Get the iterator to the start of the underlying data.

- Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) Data() const
  __aicore__ inline constexpr decltype(auto) Data()
  ```

- Return Value

    Returns the result of **Engine().Begin()**, that is, the (constant) reference or value of the iterator to the start of the data.



### Shape()

- Description

    Get the **Shape** information corresponding to the tensor.

- Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) Shape() const
  ```

- Return Value

    Returns the **Shape** object in **Layout**.



### Stride()

- Description

    Gets the **Stride** information corresponding to the tensor.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) Stride() const
  ```

- Return Value

    Returns the **Stride** object in **Layout**.



### Size()

- Description

    Gets the total number of logical elements covered by the tensor.

- Prototype

  ```cpp
  __aicore__ inline constexpr auto Size() const
  ```

- Return Value

    Returns the result of `Layout().Size()`.



### Capacity()

- Description

    Get the actual capacity occupied by the tensor under the current layout.

- Prototype

  ```cpp
  __aicore__ inline constexpr auto Capacity() const
  ```

- Return Value

    Returns the result of `Layout().Capacity()`.



### operator[]

- Description

    Accesses a single element by multidimensional coordinates.

- Prototype

  ```cpp
  template <typename Coord>
  __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord)

  template <typename Coord>
  __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord) const
  ```

- Parameters

  | Parameter | Input/Output | Description |
  | :----- | :------- | :------- |
  | **coord** | Input | Multidimensional logical coordinates. |

- Constraints
    This API is supported only in the `__gm__` and `__ubuf__` address spaces. The address spaces on the AI Core (`__ca__`, `__cb__`, `__cc__`, `__cbuf__`) do not support direct scalar access.

- Return Value

    Returns the element reference corresponding to `Data()[Layout()(coord)]`.



### operator()

- Description

    Returns a sub-tensor starting from the current position, using the given coordinates as the starting point.

- Prototype

  ```cpp
  template <typename Coord>
  __aicore__ inline constexpr decltype(auto) operator()(const Coord& coord)

  template <typename Coord>
  __aicore__ inline constexpr decltype(auto) operator()(const Coord& coord) const

  template <typename Coord0, typename Coord1, typename... Coords>
  __aicore__ inline constexpr decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs)

  template <typename Coord0, typename Coord1, typename... Coords>
  __aicore__ inline constexpr decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs) const
  ```

- Parameters

  | Parameter | Input/Output | Description |
  | :----- | :------- | :------- |
  | coord | Input | Starting coordinate tuple. |
  | c0, c1, cs... | Input | Multiple coordinate parameters, which are wrapped as **MakeCoord(c0, c1, cs...)**. |

- Return Value

    Returns a sub-tensor object starting from the specified coordinate position.



### Slice()

- Description

    Slices a sub-tensor with new layout information from the specified coordinate.

- Prototype

  ```cpp
  template <typename Coord, typename Info>
  __aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info)

  template <typename Coord, typename Info>
  __aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info) const
  ```

- Parameters

  | Parameter | Input/Output | Description |
  | :----- | :------- | :------- |
  | coord | Input | Slicing start coordinate. |
  | info | Input | Slicing description information, which can be **Shape** or **Layout**. |

- Return Value

    Returns the sliced sub-tensor object. The **Engine** of the new tensor points to the slicing start position, and its **Layout** is generated by **MakeSliceLayout(coord, Layout(), info)**.



### SetL2CacheHint()

- Description

    Sets the L2 cache hint when **GlobalTensor** accesses GM.

- Prototype

  ```cpp
  __aicore__ inline constexpr void SetL2CacheHint(Te::CacheMode mode)
  ```

- Parameters

  | Parameter | Input/Output | Description |
  | :----- | :------- | :------- |
  | mode | Input | L2 cache mode. |

- Return Value

    None.

- Constraints

    This API exists only in **GlobalTensor**; **LocalTensor** does not provide it.

## Example

```cpp
using namespace AscendC::Te;
constexpr uint64_t gmAddr = 128;

// Example 1: Construct a GM tensor.
auto gmLayout = MakeLayout(MakeShape(128, 128), MakeStride(128, 1));
auto gmTensor = MakeTensor(MakeMemPtr<Location::GM, float>(gmAddr), gmLayout);

auto gmShape = gmTensor.Shape();        // (128, 128)
auto gmStride = gmTensor.Stride();      // (128, 1)
auto gmSize = gmTensor.Size();          // 16384
auto gmCap = gmTensor.Capacity();       // 16384
gmTensor.SetL2CacheHint(CacheMode::CACHE_MODE_NORMAL);

// Example 2: Access elements by coordinates.
auto coord = MakeCoord(5, 10);
auto value = gmTensor[coord];

// Example 3: Obtain a sub-tensor.
auto subTensor = gmTensor(MakeCoord(16, 16));

// Example 4: Slice by shape.
auto sliceTensor = gmTensor.Slice(MakeCoord(0, 0), MakeShape(32, 32));

// Example 5: Construct a LocalTensor.
auto localLayout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(32, 32);
auto localTensor = MakeTensor(MakeMemPtr<Location::L1, float>(l1Addr), localLayout);
auto localData = localTensor.Data();
```

# tensor

<!-- md-trans-meta sourceCommit=2fe3a64c1bb11110e316a708c0d8e9dc67631487 translatedAt=2026-10-27T04:25:13.234Z pushedAt=2026-10-27T13:03:35.495Z nl=2 -->

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

Tensor is used to bind the memory access engine with the layout. engine describes the physical storage location and access method, while layout describes the shape and layout of the data. Tensor is divided into `global_tensor` and `local_tensor`, where `global_tensor` stores global data in Global Memory (external storage), and `local_tensor` stores data in Local Memory (internal storage) on the AI Core. The common APIs of the two are provided by `base_tensor<EngineT, LayoutT>`.

## Prototype

```cpp
enum class cache_mode : uint8_t { normal = 0, disable = 4, last = 5, persistent = 6 };

template <typename EngineT, typename LayoutT>
struct base_tensor {
    using iterator = typename EngineT::iterator;
    using value_type = typename EngineT::value_type;
    using element_type = typename EngineT::element_type;
    using reference = typename EngineT::reference;
    using engine_type = EngineT;
    using layout_type = LayoutT;

    static constexpr int rank = LayoutT::rank_size;

    __aicore__ inline constexpr base_tensor();
    __aicore__ inline constexpr base_tensor(const EngineT& engine, const LayoutT& layout);

    __aicore__ inline constexpr decltype(auto) tensor() const;
    __aicore__ inline constexpr decltype(auto) engine() const;
    __aicore__ inline constexpr decltype(auto) engine();
    __aicore__ inline constexpr decltype(auto) layout() const;
    __aicore__ inline constexpr decltype(auto) data() const;
    __aicore__ inline constexpr decltype(auto) data();
    __aicore__ inline constexpr decltype(auto) shape() const;
    __aicore__ inline constexpr decltype(auto) stride() const;
    __aicore__ inline constexpr auto size() const;
    __aicore__ inline constexpr auto capacity() const;

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord);

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord) const;

    template <typename Coord>
    __aicore__ inline decltype(auto) operator()(const Coord& coord);

    template <typename Coord>
    __aicore__ inline decltype(auto) operator()(const Coord& coord) const;

    template <typename Coord0, typename Coord1, typename... Coords>
    __aicore__ inline decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs);

    template <typename Coord0, typename Coord1, typename... Coords>
    __aicore__ inline decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs) const;

    template <typename Coord, typename Info>
    __aicore__ inline decltype(auto) slice(const Coord& coord, const Info& info);

    template <typename Coord, typename Info>
    __aicore__ inline decltype(auto) slice(const Coord& coord, const Info& info) const;

};

template <typename EngineT, typename LayoutT>
struct global_tensor : public base_tensor<EngineT, LayoutT> {
    using tensor_api_base = base_tensor<EngineT, LayoutT>;
    using tensor_api_base::tensor_api_base;

    __aicore__ inline constexpr global_tensor();
    __aicore__ inline constexpr cache_mode get_cache_mode() const;
    __aicore__ inline constexpr void set_l2_cache_hint(cache_mode mode);
};

template <typename EngineT, typename LayoutT>
struct local_tensor : public base_tensor<EngineT, LayoutT> {
    using tensor_api_base = base_tensor<EngineT, LayoutT>;
    using element_type = typename tensor_api_base::element_type;
    using data_type = get_attribute_element_type<element_type*>;

    __aicore__ inline constexpr local_tensor();
    __aicore__ inline constexpr local_tensor(const EngineT& engine, const LayoutT& layout);
};
```

## Parameters

Template Parameter Description

**Table 1** Template parameter description

| Parameter | Type | Description |
|--------|------|------|
| EngineT | Input | Memory access engine type of the tensor. |
| LayoutT | Input | Layout type of the tensor, used to describe shape and stride. |

## Return Value

The Tensor-related types themselves are type definitions and do not directly return a value.

When constructing a tensor through make_tensor: if it stores global data in Global Memory (external storage), global_tensor is returned; if it stores data in Local Memory (internal storage) on the AI Core, local_tensor is returned.

## Member Functions

### tensor()

- Description

    Obtain the tensor object itself.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) tensor() const
  ```

- Return Value

    Returns a constant reference to the current Tensor object.

---

### engine()

- Description

    Get the Engine object bound to the tensor.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) engine() const
  __aicore__ inline constexpr decltype(auto) engine()
  ```

- Return Value

    Returns a (constant) reference to the Engine object.

---

### layout()

- Description

    Get the layout object bound to the tensor.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) layout() const
  ```

- Return Value

    Returns a constant reference to the layout object.

---

### data()

- Description

    Obtains the iterator to the start of the underlying data.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) data() const
  __aicore__ inline constexpr decltype(auto) data()
  ```

- Return Value

    Returns the result of `engine().begin()`, that is, the (constant) reference or value of the iterator to the start of the data.

---

### shape()

- Description

    Get the shape information of the tensor.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) shape() const
  ```

- Return Value

    Returns the shape object in the layout.

---

### stride()

- Description

    Get the stride information corresponding to the tensor.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr decltype(auto) stride() const
  ```

- Return Value

    Returns the stride object in the layout.

---

### size()

- Description

    Gets the total number of logical elements covered by the tensor.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr auto size() const
  ```

- Return Value

    Returns the result of `layout().size()`.

---

### capacity()

- Description

    Get the actual capacity occupied by the tensor under the current layout.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr auto capacity() const
  ```

- Return Value

    Returns the result of `layout().capacity()`.

---

### operator[]

- Description

    Accesses a single element by multi-dimensional coordinates.

- Function Prototype

  ```cpp
  template <typename Coord>
  __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord)

  template <typename Coord>
  __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord) const
  ```

- Parameters

**Table 2**  Parameters

  | Parameter | Input/Output | Description |
  | :----- | :------- | :------- |
  | coord | Input | Multidimensional logical coordinates. |

- Constraints
    This API is supported only in the \_\_gm_\_ and \_\_ubuf_\_ address spaces. The address spaces on the AI Core (\_\_ca_\_, \_\_cb_\_, \_\_cc_\_, and \_\_cbuf_\_) do not support direct scalar access.

- Return Value

    Returns the element reference corresponding to `data()[layout()(coord)]`.

---

### operator()

- Description

    Returns a sub-tensor starting from the current position, with the given coordinates as the starting point.

- Function Prototype

  ```cpp
  template <typename Coord>
  __aicore__ inline decltype(auto) operator()(const Coord& coord)

  template <typename Coord>
  __aicore__ inline decltype(auto) operator()(const Coord& coord) const

  template <typename Coord0, typename Coord1, typename... Coords>
  __aicore__ inline decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs)

  template <typename Coord0, typename Coord1, typename... Coords>
  __aicore__ inline decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs) const
  ```

- Parameters

**Table 3**  operator() parameter description

  | Parameter | Input/Output | Description |
  | :----- | :------- | :------- |
  | coord | Input | Start coordinate tuple. |
  | c0, c1, cs... | Input | Multiple coordinate parameters, which are wrapped as make_coord(c0, c1, cs...). |

- Return Value

    Returns the sub-tensor object starting from the specified coordinate position.

---

### slice()

- Description

    Slices out a sub-tensor with new layout information from the specified coordinate.

- Function Prototype

  ```cpp
  template <typename Coord, typename Info>
  __aicore__ inline decltype(auto) slice(const Coord& coord, const Info& info)

  template <typename Coord, typename Info>
  __aicore__ inline decltype(auto) slice(const Coord& coord, const Info& info) const
  ```

- Parameters

**Table 4**  slice() API parameter description

  | Parameter | Input/Output | Description |
  | :----- | :------- | :------- |
  | coord | Input | Start coordinate of the slice. |
  | info | Input | Slice description information, which can be a shape or a layout. |

- Return Value

    Returns the sliced sub-tensor object. The engine of the new tensor points to the start position of the slice, and the layout is generated based on `coord` and `info`.

---

### get_cache_mode()

- Description

    Obtains the L2 cache hint currently configured for the global_tensor.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr cache_mode get_cache_mode() const
  ```

- Return Value

    Returns the current cache mode, of type `cache_mode`.

- Constraints

    This API is available only in global_tensor; local_tensor does not provide it.

---

### set_l2_cache_hint()

- Description

    Sets the L2 cache hint when global_tensor accesses GM.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr void set_l2_cache_hint(cache_mode mode)
  ```

- Parameters

**Table 5**  Parameter description of set_l2_cache_hint()

  | Parameter | Input/Output | Description |
  | :----- | :------- | :------- |
  | mode | Input | L2 cache mode. |

- Return Value

    None

- Constraints

    This API is available only in global_tensor; local_tensor does not provide it.

---

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// gm_addr is the address offset in Global Memory; make_mem_ptr<location::gm, float> marks it as
// a pointer adapter at GM location with element type float (see make_mem_ptr constraints for address and alignment requirements).
constexpr uint64_t gm_addr = 128;

// Example 1: Construct a GM tensor.
// shape is (128, 128) and stride is (128, 1): row-major contiguous layout, with adjacent elements spaced 1 element apart (unit: number of elements).
auto gm_layout = make_layout(make_shape(128, 128), make_stride(128, 1));
auto gm_tensor = make_tensor(make_mem_ptr<location::gm, float>(gm_addr), gm_layout);

auto gm_shape = gm_tensor.shape();        // (128, 128)
auto gm_stride = gm_tensor.stride();      // (128, 1)
auto gm_size = gm_tensor.size();          // 16384
auto gm_cap = gm_tensor.capacity();       // 16384
gm_tensor.set_l2_cache_hint(cache_mode::normal);

// Example 2: Access elements by coordinates.
auto coord = make_coord(5, 10);
auto value = gm_tensor[coord];

// Example 3: Obtain a sub-tensor.
auto sub_tensor = gm_tensor(make_coord(16, 16));

// Example 4: Slice by shape.
auto slice_tensor = gm_tensor.slice(make_coord(0, 0), make_shape(32, 32));

// Example 5: Construct a local_tensor.
auto local_layout = make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(32, 32);
auto local_tensor = make_tensor(make_mem_ptr<location::l1, float>(l1_addr), local_layout);
auto local_data = local_tensor.data();
```

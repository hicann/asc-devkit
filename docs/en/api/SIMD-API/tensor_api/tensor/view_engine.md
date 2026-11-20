# view_engine

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T04:25:11.723Z pushedAt=2026-10-27T13:03:35.491Z nl=2 -->

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

view_engine is the view engine in the Tensor API. It holds a pointer object Iterator and returns the starting position of this view through `begin()`. In `local_tensor`, view_engine and layout together form the view of a Tensor.

```cpp
local_tensor<view_engine<Iterator>, LayoutT>
```

## Function Prototype

  `view_engine` is defined as follows:

  ```cpp
  template <typename Iterator>
  struct view_engine
  {
      using iterator    = Iterator;
      using reference = typename iter_ref<iterator>::type;
      using element_type = typename iter_ele<iterator>::type;
      using value_type = typename iter_val<iterator>::type;

      __aicore__ inline constexpr iterator const& begin() const;
      __aicore__ inline constexpr iterator& begin();
      __aicore__ inline constexpr view_engine(iterator storage = {});
      __aicore__ inline constexpr view_engine(iterator storage, cache_mode mode);

      __aicore__ inline constexpr uint8_t get_cache_mode() const;
      __aicore__ inline constexpr void set_cache_mode(cache_mode mode);

      template <typename Index>
      __aicore__ inline constexpr view_engine operator+(const Index& i) const;

  private:
      iterator storage;
      cache_mode mode = cache_mode::normal;
  };
  ```

## Member Function Description

### `view_engine()`

- Description

  Constructs a view_engine object from the given pointer/iterator object.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr view_engine(iterator storage = {});

  ```

- Parameters

- `storage`: Pointer/iterator object corresponding to the start position of the view.

- Return Value

  The constructor has no return value.

### `begin()`

- Description

  Obtains the starting position pointer/iterator used when constructing the view_engine object.

- Function Prototype

  ```cpp
  __aicore__ inline constexpr iterator const& begin() const;

  __aicore__ inline constexpr iterator& begin();

  ```

- Return Value

- Returns the storage saved during construction, that is, the starting pointer/iterator of the current view.

## Constraints

The passed iterator object must be a valid random-access iterator to ensure that data can be correctly accessed and manipulated.

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr int32_t size = 256;
__ubuf__ float data[size];
// data is a __ubuf__ float array.
auto ptr = make_mem_ptr(data);

auto layout = make_layout(make_shape(2, 2), make_stride(2, 1));
auto tensor = make_tensor(ptr, layout);

auto engine = tensor.engine();
auto begin = engine.begin();

float a = begin[0];
float b = tensor[make_coord(1, 1)];

```

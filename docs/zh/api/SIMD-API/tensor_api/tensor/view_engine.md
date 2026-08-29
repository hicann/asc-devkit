# view_engine

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

view_engine是Tensor API中的视图引擎，负责持有一个指针对象Iterator，通过`begin()`返回这段视图的起始位置。在`local_tensor`中，view_engine和layout一起组成一个Tensor的视图。

```cpp
local_tensor<view_engine<Iterator>, LayoutT>
```

## 函数原型

  `view_engine`定义如下：

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

## 成员函数说明

### `view_engine()`

- 功能说明

  根据给定的指针/迭代器对象构造view_engine对象。

- 函数原型

  ```cpp
  __aicore__ inline constexpr view_engine(iterator storage = {});

  ```

- 参数说明

- `storage`：视图起始位置对应的指针/迭代器对象。

- 返回值说明

  构造函数无返回值。

### `begin()`

- 功能说明

  获取构造view_engine对象时的起始位置指针/迭代器。

- 函数原型

  ```cpp
  __aicore__ inline constexpr iterator const& begin() const;

  __aicore__ inline constexpr iterator& begin();

  ```

- 返回值说明

- 返回构造时保存的storage，即当前视图的起始指针/迭代器。

## 约束说明

传入的iterator对象必须是有效的随机访问迭代器，以确保能够正确访问和操作数据。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

  constexpr int32_t size = 256;
  __ubuf__ float data[size];
  auto ptr = make_mem_ptr(data);

  auto layout = make_layout(make_shape(2, 2), make_stride(2, 1));
  auto tensor = make_tensor(ptr, layout);

  auto engine = tensor.engine();
  auto begin = engine.begin();

  float a = begin[0];
  float b = tensor[make_coord(1, 1)];

```

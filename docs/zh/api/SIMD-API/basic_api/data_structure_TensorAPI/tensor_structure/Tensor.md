# Tensor

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

需要包含的头文件为：#include "tensor_api/tensor.h"。

Tensor用于将内存访问引擎Engine与布局Layout绑定起来。Engine描述物理存储位置和访问方式，Layout描述数据的形状和布局。Tensor分为GlobalTensor和LocalTensor两种，其中GlobalTensor用来存放Global Memory（外部存储）的全局数据，LocalTensor用于存放AI Core中Local Memory（内部存储）的数据。二者的公共接口由BaseTensor<TensorAttribute<EngineType, LayoutType>>提供。

## 原型定义

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

## 参数说明

模板参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| EngineType | 输入 | 张量的内存访问引擎类型。 |
| LayoutType | 输入 | 张量的布局类型，用于描述Shape和Stride。 |
| T | 输入 | 泛型模板参数，用于基础模板或特化匹配。 |

## 返回值说明

Tensor相关类型本身为类型定义，不直接返回值。

通过MakeTensor构造张量时：若存放Global Memory（外部存储）的全局数据，则返回GlobalTensor，存放AI Core中Local Memory（内部存储）的数据则返回LocalTensor。

## 成员函数

### Tensor()

- 功能说明

    获取张量对象自身。

- 函数原型

  ```cpp
  __aicore__ inline constexpr decltype(auto) Tensor() const
  ```

- 返回值说明

    返回当前Tensor对象的常量引用。

---

### Engine()

- 功能说明

    获取张量绑定的Engine对象。

- 函数原型

  ```cpp
  __aicore__ inline constexpr decltype(auto) Engine() const
  __aicore__ inline constexpr decltype(auto) Engine()
  ```

- 返回值说明

    返回Engine对象的（常量）引用。

---

### Layout()

- 功能说明

    获取张量绑定的Layout对象。

- 函数原型

  ```cpp
  __aicore__ inline constexpr decltype(auto) Layout() const
  ```

- 返回值说明

    返回Layout对象的常量引用。

---

### Data()

- 功能说明

    获取底层数据起始迭代器。

- 函数原型

  ```cpp
  __aicore__ inline constexpr decltype(auto) Data() const
  __aicore__ inline constexpr decltype(auto) Data()
  ```

- 返回值说明

    返回`Engine().Begin()`的结果，即数据起始迭代器的（常量）引用或值。

---

### Shape()

- 功能说明

    获取张量对应的Shape信息。

- 函数原型

  ```cpp
  __aicore__ inline constexpr decltype(auto) Shape() const
  ```

- 返回值说明

    返回Layout中的Shape对象。

---

### Stride()

- 功能说明

    获取张量对应的Stride信息。

- 函数原型

  ```cpp
  __aicore__ inline constexpr decltype(auto) Stride() const
  ```

- 返回值说明

    返回Layout中的Stride对象。

---

### Size()

- 功能说明

    获取张量覆盖的逻辑元素总数。

- 函数原型

  ```cpp
  __aicore__ inline constexpr auto Size() const
  ```

- 返回值说明

    返回`Layout().Size()`的计算结果。

---

### Capacity()

- 功能说明

    获取张量在当前布局下占用的实际容量。

- 函数原型

  ```cpp
  __aicore__ inline constexpr auto Capacity() const
  ```

- 返回值说明

    返回`Layout().Capacity()`的计算结果。

---

### operator[]

- 功能说明

    按多维坐标访问单个元素。

- 函数原型

  ```cpp
  template <typename Coord>
  __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord)

  template <typename Coord>
  __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord) const
  ```

- 参数说明

  | 参数名 | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | coord | 输入 | 多维逻辑坐标。 |

- 约束说明
    仅\_\_gm_\_、 \_\_ubuf_\_地址空间支持使用该接口，AI Core上的地址空间（\_\_ca_\_、\_\_cb_\_、\_\_cc_\_、\_\_cbuf_\_）不具备标量直接访问能力。

- 返回值说明

    返回Data()[Layout()(coord)]对应的元素引用。

---

### operator()

- 功能说明

    以给定坐标为起点，返回一个从当前位置开始的子张量。

- 函数原型

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

- 参数说明

  | 参数名 | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | coord | 输入 | 起始坐标元组。 |
  | c0, c1, cs... | 输入 | 多个坐标参数，会被包装为MakeCoord(c0, c1, cs...)。 |

- 返回值说明

    返回从指定坐标位置开始的子张量对象。

---

### Slice()

- 功能说明

    从指定坐标处切出一个带新布局信息的子张量。

- 函数原型

  ```cpp
  template <typename Coord, typename Info>
  __aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info)

  template <typename Coord, typename Info>
  __aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info) const
  ```

- 参数说明

  | 参数名 | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | coord | 输入 | 切片起始坐标。 |
  | info | 输入 | 切片描述信息，可以为Shape或Layout。 |

- 返回值说明

    返回切片后的子张量对象。新张量的Engine指向切片起始位置，Layout由MakeSliceLayout(coord, Layout(), info)生成。

---

### SetL2CacheHint()

- 功能说明

    设置GlobalTensor访问GM时的L2 Cache Hint。

- 函数原型

  ```cpp
  __aicore__ inline constexpr void SetL2CacheHint(Te::CacheMode mode)
  ```

- 参数说明

  | 参数名 | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | mode | 输入 | L2缓存模式。 |

- 返回值说明

    无返回值。

- 约束说明

    该接口仅存在于GlobalTensor中，LocalTensor不提供此接口。

## 调用示例

```cpp
using namespace AscendC::Te;
constexpr uint64_t gmAddr = 128;

// 示例1：构造一个GM张量
auto gmLayout = MakeLayout(MakeShape(128, 128), MakeStride(128, 1));
auto gmTensor = MakeTensor(MakeMemPtr<Location::GM, float>(gmAddr), gmLayout);

auto gmShape = gmTensor.Shape();        // (128, 128)
auto gmStride = gmTensor.Stride();      // (128, 1)
auto gmSize = gmTensor.Size();          // 16384
auto gmCap = gmTensor.Capacity();       // 16384
gmTensor.SetL2CacheHint(CacheMode::CACHE_MODE_NORMAL);

// 示例2：按坐标访问元素
auto coord = MakeCoord(5, 10);
auto value = gmTensor[coord];

// 示例3：获取子张量
auto subTensor = gmTensor(MakeCoord(16, 16));

// 示例4：按Shape切片
auto sliceTensor = gmTensor.Slice(MakeCoord(0, 0), MakeShape(32, 32));

// 示例5：构造一个LocalTensor
auto localLayout = MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(32, 32);
auto localTensor = MakeTensor(MakeMemPtr<Location::L1, float>(l1Addr), localLayout);
auto localData = localTensor.Data();
```

# Layout

> [!NOTE]说明
>本接口为试验接口，在后续版本中可能会调整或改进，不保证后续兼容性。请开发者在使用过程中关注后续版本更新。

Layout<Shape, Stride>数据结构是描述多维张量内存布局的基础模板类，通过编译时的形状Shape和步长Stride信息，实现逻辑坐标空间到一维内存地址空间的映射。借助模板元编程技术，该类在编译时完成计算和代码生成，从而降低运行时开销。

Layout包含两个核心组成部分：

- **Shape**：定义数据的逻辑形状，例如二维矩阵的行数和列数或多维张量的各维度大小。
- **Stride**：定义各维度在内存中的步长，即同维度相邻元素在内存中的间隔，单位为元素，并与Shape的维度信息一一对应。

例如，一个二维矩阵的Shape为(4, 2)，Stride为(4, 1)，表示：

- 矩阵有4行2列。
- 列方向步长为1，即每行中相邻元素间隔1个元素；行方向步长为4，即相邻行的起始地址间隔4个元素。

**表1** 线性地址视图

| 地址 | 0 | 1 | 2 | 4 | 5 | 6 | 8 | 9 | 10 | 12 | 13 |
|------|---|---|---|---|---|---|---|---|----|----|----|
| 元素 | a00 | a01 | - | a10 | a11 | - | a20 | a21 | - | a30 | a31 |

**表2** 矩阵逻辑视图

| 索引 | 列0 | 列1 |
|------|------|------|
| 行0 | a00（地址0） | a01（地址1） |
| 行1 | a10（地址4） | a11（地址5） |
| 行2 | a20（地址8） | a21（地址9） |
| 行3 | a30（地址12） | a31（地址13） |

## 需要包含的头文件

```cpp
#include "tensor_api/tensor.h"
```

## 原型定义

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

## 模板参数

| 参数名 | 描述 |
|--------|------|
| T | Std::tuple结构类型，用于定义数据的逻辑形状。 |
| U | Std::tuple结构类型，用于定义各维度在内存中的步长，并与Shape的维度信息一一对应。 |
| Info | 可选模板参数，默认为Std::ignore_t。当Layout由MakeFrameLayout或相关接口构造时，该参数可携带布局模式和布局特征等附加信息。 |

## 成员函数

### Layout()

- 功能说明

    构造Layout对象，使用给定的Shape和Stride初始化布局信息。

- 函数原型

```cpp
  __aicore__ inline constexpr Layout(const T& shape = {}, const U& stride = {})
```

- 参数说明

  | 参数名 | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | shape | 输入 | 张量的形状信息，必须为Std::tuple类型。 |
  | stride | 输入 | 张量的步长信息，必须为Std::tuple类型。 |

- 返回值说明

    构造函数，无返回值。

---

### Capacity()

- 功能说明

    获取当前Layout布局所需的实际内存容量。

    传入模板参数I...时，会先选取指定子维度，再计算对应子布局的容量。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Capacity() const
  ```

- 返回值说明

    返回当前Layout或指定子维度对应布局的实际容量。

---

### layout()

- 功能说明

    获取Layout对象自身，用于统一接口中返回Layout布局实例。

- 函数原型

  ```cpp
  __aicore__ inline constexpr decltype(auto) layout()
  __aicore__ inline constexpr decltype(auto) layout() const
  ```

- 返回值说明

    返回当前Layout对象的（常量）引用。

---

### Shape()

- 功能说明

    获取Layout中的Shape信息。

    不指定模板参数I...时返回完整Shape；传入I...时返回指定子维度对应的子结构。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Shape()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Shape() const
  ```

- 返回值说明

    返回Shape对象或其子结构的（常量）引用。

---

### Stride()

- 功能说明

    获取Layout中的Stride信息。

    不指定模板参数I...时返回完整Stride；传入I...时返回指定子维度对应的子结构。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Stride()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Stride() const
  ```

- 返回值说明

    返回Stride对象或其子结构的（常量）引用。

---

### operator()

- 功能说明

    将多维逻辑坐标映射为一维线性索引。

    该接口内部调用[Crd2Idx](../../aux_data_structures/Coordinate/Crd2Idx.md)，根据当前Layout的Shape和Stride计算坐标在内存中的线性位置。

- 函数原型

  ```cpp
  template <typename S>
  __aicore__ inline constexpr auto operator()(const S& coord) const
  ```

- 参数说明

  | 参数名 | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | coord | 输入 | 多维逻辑坐标。 |

- 返回值说明

    返回坐标coord对应的一维线性索引。

---

### Rank()

- 功能说明

    获取Layout的秩，即维度数。

    不指定模板参数I...时返回完整Layout的秩；传入I...时返回指定子维度的秩。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Rank() const
  ```

- 返回值说明

    返回当前Layout或指定子维度的秩。

---

### Size()

- 功能说明

    获取Layout覆盖的元素总数。

    不指定模板参数I...时返回完整Shape对应的元素总数；传入I...时返回指定子维度下的元素总数。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Size() const
  ```

- 返回值说明

    返回当前Layout或指定子维度覆盖的元素总数。

---

### Get()

- 功能说明

    从Layout中提取指定位置的元素或子结构。

    位置0对应Shape，位置1对应Stride；配合模板参数I...可进一步提取子tuple中的元素。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Get()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) Get() const
  ```

- 返回值说明

    返回指定位置元素或子结构的（常量）引用。

---

### depth

- 功能说明

    编译期常量，表示Shape类型的嵌套深度，用于判断当前Layout是否为嵌套布局结构。

---

### rank

- 功能说明

    编译期常量，表示完整Layout的秩。

## Layout及成员函数示例

下面通过一个二维`Layout`示例，说明`Layout`对象本身以及各成员函数的使用方式和返回结果。

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

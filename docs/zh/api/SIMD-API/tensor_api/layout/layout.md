# layout

`layout<shape, stride>`数据结构是描述多维张量内存布局的基础模板类，通过编译期的形状和步长信息，实现逻辑坐标空间到一维内存地址空间的映射。借助模板元编程技术，该类在编译期完成计算和代码生成，从而降低运行时开销。

layout包含两个核心组成部分：

- **shape**：定义数据的逻辑形状，例如二维矩阵的行数和列数或多维张量的各维度大小。
- **stride**：定义各维度在内存中的步长，即同维度相邻元素在内存中的间隔，单位为元素，并与shape的维度信息一一对应。

例如，一个二维矩阵的shape为(4, 2)，stride为(4, 1)，表示：

- 矩阵有4行2列。
- 列方向步长为1，即每行中相邻元素间隔1个元素；行方向步长为4，即相邻行的起始地址间隔4个元素。

**表1**  线性地址视图

| 地址 | 0 | 1 | 2 | 4 | 5 | 6 | 8 | 9 | 10 | 12 | 13 |
|------|---|---|---|---|---|---|---|---|----|----|----|
| 元素 | a00 | a01 | - | a10 | a11 | - | a20 | a21 | - | a30 | a31 |

**表2**  矩阵逻辑视图

| 索引 | 列0 | 列1 |
|------|------|------|
| 行0 | a00（地址0） | a01（地址1） |
| 行1 | a10（地址4） | a11（地址5） |
| 行2 | a20（地址8） | a21（地址9） |
| 行3 | a30（地址12） | a31（地址13） |

头文件路径为：`tensor_api/tensor.h`。

## 原型定义

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

## 模板参数

**表3**  模板参数说明

| 参数名 | 描述 |
|--------|------|
| ShapeTuple | Std::tuple结构类型，用于定义数据的逻辑形状。 |
| StrideTuple | Std::tuple结构类型，用于定义各维度在内存中的步长，并与shape的维度信息一一对应。 |
| Info | 可选模板参数，默认为Std::ignore_t。当layout由make_frame_layout或相关接口构造时，该参数可携带布局模式和布局特征等附加信息。 |

## 成员函数

### layout()

- 功能说明

    构造layout对象，使用给定的shape和stride初始化布局信息。

- 函数原型

```cpp
  __aicore__ inline constexpr layout(
      const ShapeTuple& shape = {}, const StrideTuple& stride = {})
```

- 参数说明

**表4**  layout()接口参数说明

  | 参数名 | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | shape | 输入 | 张量的形状信息，必须为Std::tuple类型。 |
  | stride | 输入 | 张量的步长信息，必须为Std::tuple类型。 |

- 返回值说明

    构造函数，无返回值。

---

### capacity()

- 功能说明

    获取当前layout布局所需的实际内存容量。

    传入模板参数I...时，会先选取指定子维度，再计算对应子布局的容量。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) capacity() const
  ```

- 返回值说明

    返回当前layout或指定子维度对应布局的实际容量。

---

### shape()

- 功能说明

    获取layout中的shape信息。

    不指定模板参数I...时返回完整shape；传入I...时返回指定子维度对应的子结构。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) shape()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) shape() const
  ```

- 返回值说明

    返回shape对象或其子结构的（常量）引用。

---

### stride()

- 功能说明

    获取layout中的stride信息。

    不指定模板参数I...时返回完整stride；传入I...时返回指定子维度对应的子结构。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) stride()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) stride() const
  ```

- 返回值说明

    返回stride对象或其子结构的（常量）引用。

---

### operator()

- 功能说明

    将多维逻辑坐标映射为一维线性索引。

    该接口内部调用[crd2idx](crd2idx.md)，根据当前layout的shape和stride计算坐标在内存中的线性位置。

- 函数原型

  ```cpp
  template <typename Coord>
  __aicore__ inline constexpr auto operator()(const Coord& coord) const
  ```

- 参数说明

**表5**  operator()接口参数说明

  | 参数名 | 输入/输出 | 描述 |
  | :----- | :------- | :------- |
  | coord | 输入 | 多维逻辑坐标。 |

- 返回值说明

    返回坐标coord对应的一维线性索引。

---

### rank()

- 功能说明

    获取layout的秩，即维度数。

    不指定模板参数I...时返回完整layout的秩；传入I...时返回指定子维度的秩。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) rank() const
  ```

- 返回值说明

    返回当前layout或指定子维度的秩。

---

### size()

- 功能说明

    获取layout覆盖的元素总数。

    不指定模板参数I...时返回完整shape对应的元素总数；传入I...时返回指定子维度下的元素总数。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) size() const
  ```

- 返回值说明

    返回当前layout或指定子维度覆盖的元素总数。

---

### get()

- 功能说明

    从layout中提取指定位置的元素或子结构。

    位置0对应shape，位置1对应stride；配合模板参数I...可进一步提取子tuple中的元素。

- 函数原型

  ```cpp
  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) get()

  template <size_t... I>
  __aicore__ inline constexpr decltype(auto) get() const
  ```

- 返回值说明

    返回指定位置元素或子结构的（常量）引用。

---

### depth

- 功能说明

    编译期常量，表示shape类型的嵌套深度，用于判断当前layout是否为嵌套布局结构。

---

### rank

- 功能说明

    编译期常量，表示完整layout的秩。

## layout及成员函数示例

下面通过一个二维`layout`示例，说明`layout`对象本身以及各成员函数的使用方式和返回结果。

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
// depth = 1

constexpr auto full_rank = decltype(layout_obj)::rank_size;
// full_rank = 2
```

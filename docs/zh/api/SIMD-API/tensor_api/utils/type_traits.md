# 类型判断工具

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3训练系列产品/Atlas A3推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2训练系列产品/Atlas A2推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

类型判断工具用于在编译期判断输入是否为Tensor API支持的Layout、Tensor、Coord、Shape或量化参数类型。

## 原型定义

```cpp
template <typename Layout>
constexpr bool is_layout_v = is_layout<Std::remove_cvref_t<Layout>>::value;

template <typename Tensor>
constexpr bool is_attr_tensor_v = is_attr_tensor<Std::remove_cvref_t<Tensor>>::value;

template <typename Coord>
constexpr bool is_valid_coord_v;

template <typename CopyShape>
constexpr bool is_valid_shape_v;

template <typename Quant>
constexpr bool is_valid_quant_v;
```

## 工具说明

**表1**  工具函数说明

| 工具 | 输入 | 结果 |
| :--- | :--- | :--- |
| `is_layout_v` | 待判断的类型 | 输入去除const、volatile和引用后为`layout`类型时返回true，否则返回false。 |
| `is_attr_tensor_v` | 待判断的类型 | 输入去除const、volatile和引用后为`global_tensor`或`local_tensor`类型时返回true，否则返回false。 |
| `is_valid_coord_v` | 待判断的类型 | 输入为Tuple类型或`zero_coord_type`时返回true，否则返回false。 |
| `is_valid_shape_v` | 待判断的类型 | 输入去除const、volatile和引用后为Tuple类型时返回true，否则返回false。 |
| `is_valid_quant_v` | 待判断的类型 | 输入为Tensor类型，或`uint64_t`的标量类型时返回true，否则返回false。 |

## 返回值说明

`is_layout_v`、`is_attr_tensor_v`、`is_valid_coord_v`、`is_valid_shape_v`和`is_valid_quant_v`返回编译期布尔值。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto tensor_layout = make_layout(make_shape(_8{}, _16{}), make_stride(_16{}, _1{}));
static_assert(is_layout_v<decltype(tensor_layout)>);

auto tensor = make_tensor(make_mem_ptr<location::gm, float>(0), tensor_layout);
static_assert(is_attr_tensor_v<decltype(tensor)>);

auto coord = make_coord(0, 0);
auto shape = make_shape(8, 16);
static_assert(is_valid_coord_v<decltype(coord)>);
static_assert(is_valid_shape_v<decltype(shape)>);
static_assert(is_valid_quant_v<uint64_t>);
```

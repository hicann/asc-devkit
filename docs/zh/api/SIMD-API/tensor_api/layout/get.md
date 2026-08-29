# get

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

头文件路径为：`tensor_api/tensor.h`。

get提供两类能力：

1. 对元组Tuple，按位置索引提取元素或逐层提取嵌套元素；
2. 对layout，分别从shape和stride中提取对应位置的子tuple，再组合为新的子layout。

## 函数原型

```cpp
template <size_t I, size_t... Is, typename Tuple>
__aicore__ inline constexpr auto get(Tuple&& t)

template <size_t... Is, typename Layout>
__aicore__ inline constexpr auto get(const Layout& layout)
```

## 参数说明

**表1**  模板参数说明

  | 参数名 | 类型 | 描述 |
|--------|------|------|
| Tuple | 输入 | 元组，shape和stride都是Tuple的别名。 |
| I | size_t | 元组重载中第一个索引。 |
| Is... | size_t | 其余索引序列，用于编译时递归选择元组或layout的子结构。 |
| Layout | 输入 | layout对象的类型。 |

**表2**  参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| t | 输入 | 待提取元素的元组对象。 |
| layout | 输入 | layout用于描述张量的布局。 |

## 返回值说明

- 对输入为Tuple类型，返回按索引链逐层提取后的元素；
- 对输入为layout类型，返回由提取后的shape和stride构成的新子layout对象。

## 约束说明

Is...必须为有效范围内的索引。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto t = Std::make_tuple(1, Std::make_tuple(2, 3));

auto v0 = get<0>(t);
// v0 = 1

auto v1 = get<1, 0>(t);
// v1 = 2

auto layout = make_layout(make_shape(make_shape(10, 20), make_shape(20, 30)), make_stride(make_stride(10, 200), make_stride(1, 4000)));

// 提取第 1 维度
auto sub_layout = get<1>(layout);
// sub_layout  = make_layout(make_shape(20, 30), make_stride(1, 4000));
```

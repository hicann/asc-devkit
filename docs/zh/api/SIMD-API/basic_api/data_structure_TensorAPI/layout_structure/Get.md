# Get

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

Get提供两类能力：

1. 对元组Tuple，按位置索引提取元素或逐层提取嵌套元素；
2. 对Layout，分别从shape和stride中提取对应位置的子tuple，再组合为新的子Layout。

## 函数原型

```cpp
template <size_t... Is, typename Tuple>
__aicore__ inline constexpr auto Get(Tuple&& t)

template <size_t... Is, typename Shape, typename Stride>
__aicore__ inline constexpr auto Get(const Layout<Shape, Stride>& layout)
```

## 参数说明

**表1** 模板参数说明

  | 参数名 | 类型 | 描述 |
|--------|------|------|
| Shape | 输入 | 组成Layout的shape的类型，即元组（tuple）类型。 |
| Stride | 输入 | 组成Layout的stride的类型，即元组（tuple）类型。 |
| Tuple | 输入 | 元组，Shape和Stride都是Tuple的别名。 |
| Is... | size_t | 索引序列，用于编译时递归选择shape和stride的子结构。 |

**表2** 参数说明

  | 参数名 | 类型 | 描述 |
|--------|------|------|
| layout | 输入 | Layout用于描述张量的布局。 |

## 返回值说明

- 对输入为Tuple类型，返回按索引链逐层提取后的元素；
- 对输入为Layout类型，返回新的子Layout对象，其ShapeType和StrideType为提取后的子类型。

## 约束说明

Is...必须为有效范围内的索引。

## 调用示例

```cpp
using namespace AscendC::Te;

auto t = Std::make_tuple(1, Std::make_tuple(2, 3));

auto v0 = Get<0>(t);
// v0 = 1

auto v1 = Get<1, 0>(t);
// v1 = 2

auto layout = MakeLayout(MakeShape(MakeShape(10, 20), MakeShape(20, 30)), MakeStride(MakeStride(10, 200), MakeStride(1, 4000)));

// 提取第 1 维度
auto subLayout = Get<1>(layout);
// subLayout  = MakeLayout(MakeShape(20, 30), MakeStride(1, 4000));
```

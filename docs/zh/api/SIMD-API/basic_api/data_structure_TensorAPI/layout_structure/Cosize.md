# Cosize

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

Cosize表示Layout陪域（codomain）的跨度大小，即逻辑坐标经Layout映射为一维索引后，所有可能索引值所处区间的长度。Cosize和Coshape在扁平与嵌套Layout场景下等价。

## 函数原型

```cpp
template <size_t... Is, typename Shape, typename Stride>
__aicore__ inline constexpr auto Cosize(const Layout<Shape, Stride>& layout)
```

## 参数说明

**表1** 模板参数说明

  | 参数名 | 类型 | 描述 |
|--------|------|------|
| Shape | 输入 | 组成Layout的shape的类型，即元组（tuple）类型。 |
| Stride | 输入 | 组成Layout的stride的类型，即元组（tuple）类型。 |
| Is... | size_t | 索引序列，可通过模板参数Is...指定子维度范围。用于编译时递归选择shape和stride的子结构。 |

**表2** 参数说明

  | 参数名 | 类型 | 描述 |
|--------|------|------|
| layout | 输入 | Layout用于描述张量的布局。 |

## 返回值说明

返回陪域空间的元素个数。

## 约束说明

Is...必须为有效范围内的索引。

## 调用示例

```cpp
using namespace AscendC::Te;

// 示例1：扁平Layout，Cosize = Coshape
auto layout = MakeLayout(MakeShape(10, 20), MakeStride(1, 100));
auto cosize = Cosize(layout);
// Coshape = (10-1)*1 + (20-1)*100 + 1 = 1910
// Cosize = TupleSize(1910) = 1910

// 示例2：指定子维度
auto cosize0 = Cosize<0>(layout);
// Coshape<0> = (10-1)*1 + 1 = 10
// Cosize<0> = TupleSize(10) = 10
```

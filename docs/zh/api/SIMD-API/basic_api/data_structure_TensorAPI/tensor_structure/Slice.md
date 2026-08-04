# Slice

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

Slice用于从张量指定坐标处切出一个子张量。根据输入的起始坐标coord和切片描述信息info，计算新的起始存储位置，并构造对应的子张量，子张量保持与原张量相同的存储位置类型。

## 函数原型

```cpp
template <typename Tensor, typename Coord, typename Info>
__aicore__ inline constexpr decltype(auto) Slice(Tensor&& tensor, const Coord& coord, const Info& info)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| Tensor | 输入 | 张量类型，通常为GlobalTensor<...>或LocalTensor<...>。 |
| Coord | 输入 | 坐标类型，通常为Coord<...>元组类型。 |
| Info | 输入 | 切片描述类型，可以为Shape类型或Layout类型。传入Shape时，表示按给定形状截取子张量；传入Layout时，表示按Layout中的Shape信息截取子张量并保持原有布局模式和Stride结构。 |

**表2** 参数说明

| 参数名 | 类型 | 描述 |
|--------|------|------|
| tensor | 输入 | 待切片的张量对象。 |
| coord | 输入 | 切片起始坐标。 |
| info | 输入 | 切片描述信息。可以传入Shape或Layout。 |

## 返回值说明

返回从coord指定位置开始的子张量对象。返回结果包含新的Layout信息，Engine指向切片后的起始位置。

## 约束说明

- coord的tuple结构必须与当前Tensor的Layout.Shape()结构一致。
- 当info为Shape时：info必须是二维Shape；当前Tensor的Layout必须为二维或四维。
- 当info为Layout时，info必须与当前Tensor的Layout的维度相同。

## 调用示例

```cpp
using namespace AscendC::Te;
constexpr uint64_t gmAddr = 128;

// 示例1：按Shape切片
auto layout1 = MakeLayout(MakeShape(10, 20), MakeStride(20, 1));
auto tensor1 = MakeTensor(MakeMemPtr<Location::GM, float>(gmAddr), layout1);
auto sub1 = Slice(tensor1, MakeCoord(2, 4), MakeShape(4, 8));
// 起始坐标为(2, 4)，子张量形状为(4, 8)

// 示例2：按Layout切片
auto layout2 = MakeLayout(MakeShape(10, 20), MakeStride(20, 1));
auto tensor2 = MakeTensor(MakeMemPtr<Location::GM, float>(gmAddr), layout2);
auto sub2 = Slice(tensor2, MakeCoord(1, 2), MakeLayout(MakeShape(3, 5), MakeStride(20, 1)));
// 从(1, 2)开始切出一个Shape为(3, 5)的子张量，并保持Stride结构

// 示例3：等价于调用成员函数
auto sub3 = tensor2.Slice(MakeCoord(1, 2), MakeShape(3, 5));
// sub3与Slice(tensor2, MakeCoord(1, 2), MakeShape(3, 5))等价
```

# MakePatternLayout

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

MakePatternLayout用于根据Shape、Stride、LayoutPattern和TraitType构造带布局模式信息的Layout对象。

与[MakeLayout](./MakeLayout.md)相比，MakePatternLayout除了保存Shape和Stride外，还会在Layout类型中附加布局模式LayoutPattern和Trait信息。

TraitType用于描述C0粒度等布局特征。若使用`LayoutTrait<T, C0>`或`LayoutTraitDefault<T>`写法，T仅作为兼容占位或C0推导依据，不作为Tensor的数据类型信息保存。

## 函数原型

```cpp
template <typename LayoutPattern, typename TraitType, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto MakePatternLayout(const ShapeType& shape, const StrideType& stride)
```

## 参数说明

**表1** 模板参数说明

| 参数名称 | 类型 | 描述 |
| :------- | :--- | :--- |
| LayoutPattern | 输入 | 布局模式类型，用于标记Layout的格式信息。 |
| TraitType | 输入 | 布局trait类型，用于标记C0粒度等布局特征。 |
| ShapeType | 输入 | shape的类型，要求为元组(tuple)类型。 |
| StrideType | 输入 | stride的类型，要求为元组(tuple)类型。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 描述 |
| :------- | :-------- | :--- |
| shape | 输入 | 描述逻辑形状的元组对象，可由[MakeShape](./MakeShape.md)构造。 |
| stride | 输入 | 描述各维步长的元组对象，可由[MakeStride](./MakeStride.md)构造。 |

## 返回值说明

返回Layout<ShapeType, StrideType, Std::tuple<LayoutPattern, TraitType>>类型对象。

## 约束说明

- shape和stride必须满足Layout构造要求，即都为元组类型。
- LayoutPattern应为合法的布局模式类型，例如NZLayoutPtn、ZNLayoutPtn、NDLayoutPtn、DNLayoutPtn等。
- TraitType应与LayoutPattern及后续使用场景匹配，否则可能导致后续格式检查或路由失败。

## 调用示例

```cpp
using namespace AscendC::Te;

// 示例1：构造带NZ布局模式的Layout
auto layout1 = MakePatternLayout<NZLayoutPtn, LayoutTraitDefault<>>(
    MakeShape(MakeShape(16, 2), MakeShape(16, 4)),
    MakeStride(MakeStride(16, 256), MakeStride(1, 512))
);

// 示例2：构造带ND布局模式的Layout
auto layout2 = MakePatternLayout<NDLayoutPtn, LayoutTrait<Std::ignore_t, _1>>(
    MakeShape(32, 64),
    MakeStride(64, 1)
);
```

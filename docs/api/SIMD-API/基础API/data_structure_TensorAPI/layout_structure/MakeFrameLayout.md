# MakeFrameLayout

> [!NOTE]说明
>本接口为试验接口，在后续版本中可能会调整或改进，不保证后续兼容性。请开发者在使用过程中关注后续版本更新。

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

MakeFrameLayout用于根据标准分形排布模式构造带类型信息的Layout对象。该接口接收矩阵的行数、列数，结合指定的LayoutPattern（排布模式）和LayoutTrait（布局特征参数）计算Shape和Stride，构造Layout对象。该接口也支持在矩阵维度前传入Batch维度，用于构造多Batch矩阵的Layout。

LayoutPattern决定数据在内存中的分形排列方式，不同存储位置间的搬运和计算操作要求源/目的张量满足特定的LayoutPattern组合。

## 函数原型

```cpp
template <typename LayoutPattern, typename TraitType, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args)
```

常用调用形式如下。

```cpp
// 构造单个矩阵Layout，m为行数，n为列数。
auto layout = MakeFrameLayout<LayoutPattern, TraitType>(m, n);

// 构造Batch矩阵Layout，batch为矩阵个数，m为单个矩阵的行数，n为单个矩阵的列数。
auto batchLayout = MakeFrameLayout<LayoutPattern, TraitType>(batch, m, n);
```

### LayoutTrait的指定方式

LayoutTrait用于指定数据类型和C0维度基数。MakeFrameLayout支持以下四种传递方式：

1. **LayoutTraitDefault<T>**：指定数据类型T，C0自动计算。

   ```cpp
   MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n)
   ```

2. **_C0**：直接指定C0维度的元素个数，数据类型为[Std::Int](../../../../Utils-API/CPP标准库/类型特性/integral_constant.md)。_C0是Std::Int<C0>的简写形式。

   ```cpp
   MakeFrameLayout<NZLayoutPtn, _16>(m, n)
   ```

3. **不传入Trait**：在分形构造和Trait无关，或Trait值固定的场景下，不传入Trait，内部根据LayoutPattern查找对应的默认Trait。这样的分形有NDLayoutPtn、DNLayoutPtn、DNExtLayoutPtn、NDExtLayoutPtn、ScaleANDLayoutPtn、ScaleADNLayoutPtn、ScaleBNDLayoutPtn、ScaleBDNLayoutPtn。

   ```cpp
   MakeFrameLayout<NDLayoutPtn>(m, n)
   ```

4. **自定义Trait传入**：传入一个自定义的Trait类型。适用于需要显式描述数据类型、C0大小或其他布局特征，且现有默认Trait、_C0或固定默认Trait都无法满足需求的场景。自定义Trait需要满足底层布局推导所要求的类型约定。

   ```cpp
   struct MyLayoutTrait {
       using type = float;
       static constexpr auto C0_ELEMENT = Std::Int<16>{};
   };

   MakeFrameLayout<NZLayoutPtn, MyLayoutTrait>(m, n)
   ```

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| LayoutPattern | 输入 | 布局模式模板参数。支持的LayoutPattern及数据排布格式见[Layout和层次化表述法](Layout和层次化表述法.md)。 |
| TraitType | 输入 | 布局特征类型，默认内部根据LayoutPattern自动推导。支持上述四种传递方式。 |
| Args | 输入 | 构造参数。常见为矩阵的行数m和列数n；Batch模式为batch、m、n。各LayoutPattern的构造参数数量可能不同。 |

## 返回值说明

返回Layout<ShapeType, StrideType, Std::tuple<LayoutPattern, TraitType>>类型的对象，其中ShapeType和StrideType由LayoutPattern和构造参数在编译期推导。

## 约束说明

- LayoutPattern必须是已支持的排布模式之一，不支持的模式会触发编译期报错。
- 构造参数的个数和含义由LayoutPattern决定。常见二维场景传入(m, n)即可。
- Batch模式传入(batch, m, n)，其中m和n表示单个矩阵的行数和列数，batch表示连续排布的矩阵个数。
- TraitType必须为整型常量、TraitType类型或具体数据类型，不支持的类型会触发编译期报错。

## 调用示例

```cpp
using namespace AscendC::Te;

// 构造GM端ND布局张量
auto layoutGM = MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n);
auto gmTensor = MakeTensor(MakeMemPtr<Location::GM>(gmAddr), layoutGM);

// 构造L1端NZ布局张量（L0A/L0B搬运的标准格式）
auto layoutL1 = MakeFrameLayout<NZLayoutPtn, float>(m, n);
auto l1Tensor = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutL1);

// 使用默认Trait构造NZ布局
auto layoutNZ = MakeFrameLayout<NZLayoutPtn>(m, n);
auto l1TensorNZ = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutNZ);

// 指定C0元素数构造
auto layoutC0 = MakeFrameLayout<NZLayoutPtn, _16>(m, n);
auto l1TensorC0 = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutC0);

// 自定义Trait构造
struct MyLayoutTrait {
    using type = float;
    static constexpr auto C0_ELEMENT = Std::Int<16>{};
};
auto layoutCustom = MakeFrameLayout<NZLayoutPtn, MyLayoutTrait>(m, n);

// 构造Batch矩阵布局
auto layoutBatchNZ = MakeFrameLayout<NZLayoutPtn, float>(batch, m, n);
auto l1BatchTensor = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutBatchNZ);

// 构造MX场景的scale Batch布局
auto layoutBatchScaleA = MakeFrameLayout<ScaleANDLayoutPtn>(batch, scaleM, scaleK);
auto gmScaleTensor = MakeTensor(MakeMemPtr<Location::GM>(scaleAddr), layoutBatchScaleA);
```

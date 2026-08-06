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

MakeFrameLayout用于根据标准分形排布模式构造带类型信息的Layout对象。该接口根据构造参数，结合指定的LayoutPattern（排布模式）和LayoutTrait（布局特征参数）计算Shape和Stride，构造Layout对象。除矩阵Layout外，该接口还支持构造卷积场景中的NCHW、NHWC和NC1HWC0格式Layout。该接口也支持在矩阵维度前传入Batch维度，用于构造多Batch矩阵的Layout。

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

// 构造卷积输入或输出Layout。
auto nchwLayout = MakeFrameLayout<NCHWLayoutPtn>(n, c, h, w);
auto nhwcLayout = MakeFrameLayout<NHWCLayoutPtn>(n, h, w, c);
auto nc1hwc0Layout = MakeFrameLayout<NC1HWC0LayoutPtn>(n, c1, h, w, c0);
```

### 卷积LayoutPattern

| LayoutPattern | 构造参数 | Shape | Stride | 描述 |
| :--- | :--- | :--- | :--- | :--- |
| `NCHWLayoutPtn` | `n, c, h, w` | `(N, C, H, W)` | `(C * H * W, H * W, W, 1)` | 通道维C位于高度维H和宽度维W之前。 |
| `NHWCLayoutPtn` | `n, h, w, c` | `(N, H, W, C)` | `(H * W * C, W * C, C, 1)` | 通道维C位于最内层。 |
| `NC1HWC0LayoutPtn` | `n, c1, h, w, c0` | `(N, C1, H, W, C0)` | `(C1 * H * W * C0, H * W * C0, W * C0, C0, 1)` | 将通道维C拆分为C1和C0，其中`C = C1 * C0`。 |

上述Layout均为行主序连续排布，Stride的单位为元素。`NC1HWC0LayoutPtn`的C0由函数参数显式指定，不通过LayoutTrait推导。

### LayoutTrait的指定方式

LayoutTrait用于指定C0维度基数。MakeFrameLayout支持以下四种传递方式：

1. **`LayoutTraitDefault<T>`**：根据数据类型T自动计算C0。T仅参与C0推导，不作为LayoutTrait中的数据类型信息保存。

   ```cpp
   MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n)
   ```

2. **C0常量类型**：直接指定C0维度的元素个数。`_8`、`_16`等类型是[Std::Int](../../../../Utils-API/cpp_stdlib/type_traits/integral_constant.md)的简写形式，例如`_16`表示`Std::Int<16>`。

   ```cpp
   MakeFrameLayout<NZLayoutPtn, _16>(m, n)
   ```

3. **省略第二个模板参数**：在分形构造不需要用户指定C0，或C0由布局模式固定的场景下，可以只传入LayoutPattern。此时接口会根据LayoutPattern使用内置的C0取值。这样的分形有NDLayoutPtn、DNLayoutPtn、DNExtLayoutPtn、NDExtLayoutPtn、ScaleANDLayoutPtn、ScaleADNLayoutPtn、ScaleBNDLayoutPtn、ScaleBDNLayoutPtn。

   ```cpp
   MakeFrameLayout<NDLayoutPtn>(m, n)
   ```

4. **自定义Trait类型**：当根据数据类型推导C0、直接传入C0常量类型、省略第二个模板参数这三种方式都无法表达目标布局时，可以传入自定义Trait类型。自定义Trait需要提供`C0_ELEMENT`成员，用于表示C0维度的元素个数。

   ```cpp
   struct MyLayoutTrait {
       static constexpr auto C0_ELEMENT = Std::Int<16>{};
   };

   MakeFrameLayout<NZLayoutPtn, MyLayoutTrait>(m, n)
   ```

为兼容已有代码，也支持`LayoutTrait<T, C0>`写法，其中T仅用于保持源码兼容，实际LayoutTrait只使用C0作为布局特征。

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| LayoutPattern | 输入 | 布局模式模板参数。支持的LayoutPattern及数据排布格式见[Layout和层次化表述法](Layout_hierarchical_representation.md)。 |
| TraitType | 输入 | 第二个模板参数，用于描述C0取值。可传入`LayoutTraitDefault<T>`、C0常量类型、带`C0_ELEMENT`成员的自定义Trait类型或具体数据类型；省略时接口按LayoutPattern使用内置的C0取值。 |
| Args | 输入 | 构造参数。矩阵场景常见为m和n，Batch矩阵场景为batch、m和n。卷积场景根据LayoutPattern传入n、c、h、w或n、c1、h、w、c0。 |

## 返回值说明

返回Layout<ShapeType, StrideType, Std::tuple<LayoutPattern, TraitType>>类型的对象，其中ShapeType和StrideType由LayoutPattern和构造参数在编译期推导。

## 约束说明

- LayoutPattern必须是已支持的排布模式之一，不支持的模式会触发编译期报错。
- 构造参数的个数和含义由LayoutPattern决定。常见二维场景传入(m, n)即可。
- 使用`NC1HWC0LayoutPtn`时，用户需要根据数据类型和后续运算要求显式传入C0，并保证C轴数据符合`C = C1 * C0`。
- Batch模式传入(batch, m, n)，其中m和n表示单个矩阵的行数和列数，batch表示连续排布的矩阵个数。
- TraitType必须为整型常量、带`C0_ELEMENT`成员的Trait类型或具体数据类型，不支持的类型会触发编译期报错。传入具体数据类型时，该类型仅用于推导C0；Tensor的真实数据类型由`MakeTensor`绑定的内存指针类型决定。

## 调用示例

```cpp
using namespace AscendC::Te;

// 构造GM端ND布局张量
auto layoutGM = MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n);
auto gmTensor = MakeTensor(MakeMemPtr<Location::GM>(gmAddr), layoutGM);

// 构造L1端NZ布局张量（L0A/L0B搬运的标准格式）
auto layoutL1 = MakeFrameLayout<NZLayoutPtn, float>(m, n);
auto l1Tensor = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutL1);

// 省略第二个模板参数构造NZ布局
auto layoutNZ = MakeFrameLayout<NZLayoutPtn>(m, n);
auto l1TensorNZ = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutNZ);

// 指定C0元素数构造
auto layoutC0 = MakeFrameLayout<NZLayoutPtn, _16>(m, n);
auto l1TensorC0 = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutC0);

// 自定义Trait构造
struct MyLayoutTrait {
    static constexpr auto C0_ELEMENT = Std::Int<16>{};
};
auto layoutCustom = MakeFrameLayout<NZLayoutPtn, MyLayoutTrait>(m, n);

// 构造Batch矩阵布局
auto layoutBatchNZ = MakeFrameLayout<NZLayoutPtn, float>(batch, m, n);
auto l1BatchTensor = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutBatchNZ);

// 构造MX场景的scale Batch布局
auto layoutBatchScaleA = MakeFrameLayout<ScaleANDLayoutPtn>(batch, scaleM, scaleK);
auto gmScaleTensor = MakeTensor(MakeMemPtr<Location::GM>(scaleAddr), layoutBatchScaleA);

// 构造卷积特征图Layout
auto layoutNCHW = MakeFrameLayout<NCHWLayoutPtn>(n, c, h, w);
auto layoutNHWC = MakeFrameLayout<NHWCLayoutPtn>(n, h, w, c);
auto layoutNC1HWC0 = MakeFrameLayout<NC1HWC0LayoutPtn>(n, c1, h, w, c0);
```

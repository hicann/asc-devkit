# MakeFrameLayout

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:53:25.254Z -->

> [!NOTE]
> This API is an experimental API. It may be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers should pay attention to later version updates during use.

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file to include is: `#include "tensor_api/tensor.h"`.

**MakeFrameLayout** constructs a **Layout** object with type information based on the standard fractal layout pattern. This API takes the number of rows and columns of a cube, and computes **Shape** and **Stride** in combination with the specified **LayoutPattern** (layout pattern) and **LayoutTrait** (layout trait parameters) to construct the **Layout** object. This API also supports passing a **Batch** dimension before the cube dimensions to construct the **Layout** of a multi-batch cube.

**LayoutPattern** determines the fractal arrangement of data in memory. Data movement and computation operations between different storage locations require the source/destination tensors to satisfy a specific **LayoutPattern** combination.

## Prototype

```cpp
template <typename LayoutPattern, typename TraitType, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args)
```

The common calling forms are as follows.

```cpp
// Construct a single-cube Layout, where m is the number of rows and n is the number of columns.
auto layout = MakeFrameLayout<LayoutPattern, TraitType>(m, n);

// Construct a batch-cube Layout, where batch is the number of matrices, m is the number of rows of a single cube, and n is the number of columns of a single cube.
auto batchLayout = MakeFrameLayout<LayoutPattern, TraitType>(batch, m, n);
```

### How to Specify LayoutTrait

**LayoutTrait** is used to specify the data type and the C0 dimension cardinality. **MakeFrameLayout** supports the following four passing methods:

1. **LayoutTraitDefault\<T\>**: Specifies the data type T, with C0 calculated automatically.

   ```cpp
   MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<float>>(m, n)
   ```

2. **_C0**: Directly specifies the number of elements in the C0 dimension, with the data type being [Std::Int](../../../../Utils-API/cpp_stdlib/type_traits/integral_constant.md). **_C0** is the shorthand form of **Std::Int\<C0\>**.

   ```cpp
   MakeFrameLayout<NZLayoutPtn, _16>(m, n)
   ```

3. **Not passing a Trait**: In scenarios where fractal construction is independent of the Trait, or where the Trait value is fixed, no Trait is passed, and the corresponding default Trait is looked up internally based on the LayoutPattern. Such fractals include **NDLayoutPtn**, **DNLayoutPtn**, **DNExtLayoutPtn**, **NDExtLayoutPtn**, **ScaleANDLayoutPtn**, **ScaleADNLayoutPtn**, **ScaleBNDLayoutPtn**, and **ScaleBDNLayoutPtn**.

   ```cpp
   MakeFrameLayout<NDLayoutPtn>(m, n)
   ```

4. **Passing a custom Trait**: Passes a custom Trait type. This applies to scenarios where the data type, C0 size, or other layout characteristics need to be explicitly described, and where the existing default Trait, **_C0**, or fixed default Trait cannot meet the requirements. The custom Trait must satisfy the type conventions required by the underlying layout derivation.

   ```cpp
   struct MyLayoutTrait {
       using type = float;
       static constexpr auto C0_ELEMENT = Std::Int<16>{};
   };

   MakeFrameLayout<NZLayoutPtn, MyLayoutTrait>(m, n)
   ```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| LayoutPattern | Input | Layout pattern template parameter. For supported **LayoutPattern** values and data layout formats, see [Layout and hierarchical representation](layout_and_hierarchical_representation.md). |
| TraitType | Input | Layout trait type, which is automatically deduced from **LayoutPattern** by default. The four passing methods above are supported. |
| Args | Input | Construction parameters. They are commonly the number of rows **m** and the number of columns **n** of a cube; in **Batch** mode, they are **batch**, **m**, and **n**. The number of construction parameters may vary with **LayoutPattern**. |

## Return Value

Returns an object of type **Layout**<**ShapeType**, **StrideType**, **Std::tuple**<**LayoutPattern**, **TraitType**>>, where **ShapeType** and **StrideType** are deduced at compile time from **LayoutPattern** and the construction parameters.

## Constraints

- **LayoutPattern** must be one of the supported layout patterns. Unsupported patterns trigger a compile-time error.
- The number and meaning of the constructor parameters are determined by **LayoutPattern**. For common two-dimensional scenarios, passing (m, n) is sufficient.
- In Batch mode, pass (batch, m, n), where m and n represent the number of rows and columns of a single cube, and batch represents the number of consecutively arranged matrices.
- **TraitType** must be an integer constant, a **TraitType** type, or a concrete data type. Unsupported types trigger a compile-time error.

## Example

```cpp
using namespace AscendC::Te;

// Construct the GM-side ND layout tensor.
auto layoutGM = MakeFrameLayout<NDExtLayoutPtn, LayoutTraitDefault<float>>(m, n);
auto gmTensor = MakeTensor(MakeMemPtr<Location::GM>(gmAddr), layoutGM);

// Construct the L1-side NZ layout tensor (the standard format for L0A/L0B transfer).
auto layoutL1 = MakeFrameLayout<NZLayoutPtn, float>(m, n);
auto l1Tensor = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutL1);

// Construct the NZ layout using the default trait.
auto layoutNZ = MakeFrameLayout<NZLayoutPtn>(m, n);
auto l1TensorNZ = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutNZ);

// Construct by specifying the number of C0 elements.
auto layoutC0 = MakeFrameLayout<NZLayoutPtn, _16>(m, n);
auto l1TensorC0 = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutC0);

// Construct with a custom trait.
struct MyLayoutTrait {
    using type = float;
    static constexpr auto C0_ELEMENT = Std::Int<16>{};
};
auto layoutCustom = MakeFrameLayout<NZLayoutPtn, MyLayoutTrait>(m, n);

// Construct the batch cube layout.
auto layoutBatchNZ = MakeFrameLayout<NZLayoutPtn, float>(batch, m, n);
auto l1BatchTensor = MakeTensor(MakeMemPtr<Location::L1>(l1Addr), layoutBatchNZ);

// Construct the scale batch layout for the MX scenario.
auto layoutBatchScaleA = MakeFrameLayout<ScaleANDLayoutPtn>(batch, scaleM, scaleK);
auto gmScaleTensor = MakeTensor(MakeMemPtr<Location::GM>(scaleAddr), layoutBatchScaleA);
```

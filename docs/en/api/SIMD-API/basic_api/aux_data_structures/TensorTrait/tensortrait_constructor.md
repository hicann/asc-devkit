# TensorTrait Constructor<a name="ZH-CN_TOPIC_0000002371314333"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:58:56.893Z -->

## Applicable Products<a name="section73648168211"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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


## Description<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section36583473819"></a>

Instantiates a **TensorTrait** object based on the input **Layout** object.

## Prototype<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section13230182415108"></a>

```cpp
__aicore__ inline TensorTrait(const LayoutType& t = {})
```

## Parameters<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="p1085176175119"><a name="p1085176175119"></a><a name="p1085176175119"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="p1851763519"><a name="p1851763519"></a><a name="p1851763519"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="p148519610515"><a name="p148519610515"></a><a name="p148519610515"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p18419104127"><a name="p18419104127"></a><a name="p18419104127"></a>**t**</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="p1641874129"><a name="p1641874129"></a><a name="p1641874129"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p14181041623"><a name="p14181041623"></a><a name="p14181041623"></a>Input <a href="../Layout/Layout.md">Layout</a> object. The input data type is **LayoutType**, which must meet the <a href="../Layout/layout_constructor.md#zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section19165124931511">Constraints</a>.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section320753512363"></a>

-   TensorTrait usage example

    ```cpp
    // The TensorTrait usage example is a UT example based on googletest.
    
    // Create a TensorTrait using the MakeTensorTrait method.
    AscendC::Shape<int,int,int> shape = AscendC::MakeShape(10, 20, 30);
    AscendC::Stride<int,int,int> stride = AscendC::MakeStride(1, 100, 200);
    auto layoutMake = AscendC::MakeLayout(shape, stride);    
    auto tensorTraitMake = AscendC::MakeTensorTrait<float, AscendC::TPosition::VECIN>(layoutMake);
    
    EXPECT_EQ(AscendC::Std::get<0>(tensorTraitMake.GetLayout().GetShape()), 10);
    EXPECT_EQ(AscendC::Std::get<1>(tensorTraitMake.GetLayout().GetShape()), 20);
    EXPECT_EQ(AscendC::Std::get<2>(tensorTraitMake.GetLayout().GetShape()), 30);
    EXPECT_EQ(AscendC::Std::get<0>(tensorTraitMake.GetLayout().GetStride()), 1);
    EXPECT_EQ(AscendC::Std::get<1>(tensorTraitMake.GetLayout().GetStride()), 100);
    EXPECT_EQ(AscendC::Std::get<2>(tensorTraitMake.GetLayout().GetStride()), 200);
    
    // Create a TensorTrait using the constructor.
    using TensorTraitType = AscendC::TensorTrait<half, AscendC::TPosition::VECCALC, AscendC::Layout<AscendC::Shape<int, int, int>, AscendC::Stride<int, int, int>>>;
    TensorTraitType tensorTraitInit(layoutMake);
    
    EXPECT_EQ(AscendC::Std::get<0>(tensorTraitInit.GetLayout().GetShape()), 10);
    EXPECT_EQ(AscendC::Std::get<1>(tensorTraitInit.GetLayout().GetShape()), 20);
    EXPECT_EQ(AscendC::Std::get<2>(tensorTraitInit.GetLayout().GetShape()), 30);
    EXPECT_EQ(AscendC::Std::get<0>(tensorTraitInit.GetLayout().GetStride()), 1);
    EXPECT_EQ(AscendC::Std::get<1>(tensorTraitInit.GetLayout().GetStride()), 100);
    EXPECT_EQ(AscendC::Std::get<2>(tensorTraitInit.GetLayout().GetStride()), 200);
    
    EXPECT_EQ(AscendC::Std::get<0>(tensorTraitInit.GetShape()), 10);
    EXPECT_EQ(AscendC::Std::get<1>(tensorTraitInit.GetShape()), 20);
    EXPECT_EQ(AscendC::Std::get<2>(tensorTraitInit.GetShape()), 30);
    EXPECT_EQ(AscendC::Std::get<0>(tensorTraitInit.GetStride()), 1);
    EXPECT_EQ(AscendC::Std::get<1>(tensorTraitInit.GetStride()), 100);
    EXPECT_EQ(AscendC::Std::get<2>(tensorTraitInit.GetStride()), 200);
    
    // Set the TensorTrait using the SetLayout method.
    TensorTraitType tensorTraitSet;
    tensorTraitSet.SetLayout(layoutMake);
    
    EXPECT_EQ(AscendC::Std::get<0>(tensorTraitSet.GetLayout().GetShape()), 10);
    EXPECT_EQ(AscendC::Std::get<1>(tensorTraitSet.GetLayout().GetShape()), 20);
    EXPECT_EQ(AscendC::Std::get<2>(tensorTraitSet.GetLayout().GetShape()), 30);
    EXPECT_EQ(AscendC::Std::get<0>(tensorTraitSet.GetLayout().GetStride()), 1);
    EXPECT_EQ(AscendC::Std::get<1>(tensorTraitSet.GetLayout().GetStride()), 100);
    EXPECT_EQ(AscendC::Std::get<2>(tensorTraitSet.GetLayout().GetStride()), 200);
    ```

-   TensorTrait used with APIs example

    ```cpp
    AscendC::LocalTensor<AscendC::TensorTrait<half>> tensor1 = que1.DeQue<AscendC::TensorTrait<half>>();
    AscendC::LocalTensor<AscendC::TensorTrait<half>> tensor2 = que2.DeQue<AscendC::TensorTrait<half>>();
    AscendC::LocalTensor<AscendC::TensorTrait<half>> tensor3 = que3.AllocTensor<AscendC::TensorTrait<half>>();
    Add(tensor3, tensor1, tensor2, tensor3.GetSize());
    ```


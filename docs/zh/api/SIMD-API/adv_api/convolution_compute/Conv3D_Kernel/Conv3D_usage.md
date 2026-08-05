# Conv3D使用说明

Ascend C提供一组Conv3D高阶API，方便用户快速实现3维卷积正向矩阵运算。3维正向卷积的示意图如[图1](#fig198021635102613)，其计算公式为：

![](../../../../figures/zh-cn_formulaimage_0000002359552057.png)

-   X为Conv3D卷积的特征矩阵Input。
-   W为Conv3D卷积的权重矩阵Weight。
-   B为Conv3D卷积的偏置矩阵Bias。
-   Y为完成卷积及偏置操作之后的结果矩阵Output。

**图1**  3维正向卷积示意图<a name="fig198021635102613"></a>  
![](../../../../figures/3维正向卷积示意图.png "3维正向卷积示意图")

> [!NOTE]说明
>Cin为Input的输入通道大小Channel；Din为Input的Depth维度大小；Hin为Input的Height维度大小；Win为Input的Width维度大小；Cout为Weight、Output的输出通道大小；Dout为Output的Depth维度的大小；Hout为Output的Height维度大小；Wout为Output的Width维度大小；下文中提及的M维度，为卷积正向操作过程中的输入Input在img2col展开后的纵轴，数值上等于Hout \* Wout。
>Channel、Depth、Height、Width后续简称为C、D、H、W。

除上述基础运算外，在Conv3D计算中可以设置参数Padding、Stride和Dilation，具体含义如下。

-   Padding代表在输入矩阵的三个维度上填充0，见[图2](#fig16852164019363)。
-   Stride代表卷积核三个维度上滑动的距离，见[图3](#fig137769564018)。
-   Dilation代表卷积核三个维度上每个数据的间距，见[图4](#fig1015315044111)。

**图2**  卷积3D正向Padding示意图<a name="fig16852164019363"></a>  
![](../../../../figures/卷积3D正向Padding示意图.png "卷积3D正向Padding示意图")

**图3**  卷积3D正向Stride示意图<a name="fig137769564018"></a>  
![](../../../../figures/卷积3D正向Stride示意图.png "卷积3D正向Stride示意图")

**图4**  卷积3D正向Dilation示意图<a name="fig1015315044111"></a>  
![](../../../../figures/卷积3D正向Dilation示意图.png "卷积3D正向Dilation示意图")

Kernel侧实现Conv3D运算的步骤概括为：

1.  创建Conv3D对象。
2.  初始化操作。
3.  设置3D卷积输入Input、Weight、Bias和输出Output。
4.  完成3D卷积操作。
5.  结束3D卷积操作。

使用Conv3D高阶API实现卷积正向的具体步骤如下：

1.  创建Conv3D对象。

    ```
    #include "lib/conv/conv3d/conv3d_api.h"

    using inputType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::NDC1HWC0, bfloat16_t>;
    using weightType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::FRACTAL_Z_3D, bfloat16_t>;
    using outputType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::NDC1HWC0, bfloat16_t>;
    using biasType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::ND, float>; // 可选参数

    Conv3dApi::Conv3D<inputType, weightType, outputType, biasType> conv3dApi;
    ```

    创建对象时需要传入Input、Weight和Output参数类型信息；Bias的参数类型为可选参数，不带Bias输入的卷积计算场景，不传入该参数。类型信息通过ConvType来定义，包括：内存逻辑位置、数据格式、数据类型。

    ```
    template <TPosition POSITION, ConvFormat FORMAT, typename TYPE>
    struct ConvType {
        constexpr static TPosition pos = POSITION;   // Conv3d输入或输出在内存上的位置
        constexpr static ConvFormat format = FORMAT; // Conv3d输入或者输出的数据格式
        using T = TYPE;                              // Conv3d输入或输出的数据类型
    };
    ```

    下面简要介绍在创建对象时使用到的相关数据结构，开发者可选择性地了解这些内容。用于创建Conv3D对象的数据结构定义如下：

    ```
    template <
        class INPUT_TYPE, class WEIGHT_TYPE, class OUTPUT_TYPE, class BIAS_TYPE = biasType, class CONV_CFG = Conv3dParam>
    using Conv3D =
        Conv3dIntfExt<Config<ConvApi::ConvDataType<INPUT_TYPE, WEIGHT_TYPE, OUTPUT_TYPE, BIAS_TYPE, CONV_CFG>>, Impl, Intf>
    ```

    其中，Conv3dIntfExt和Conv3dParam数据结构定义如下：

    ```
    template <
        class Conv3dCfg, template <typename, class, bool> class Impl = Conv3dApiImpl,
        template <class, template <typename, class, bool> class> class Intf = Conv3dIntf>
    struct Conv3dIntfExt : public Intf<Conv3dCfg, Impl> {
        __aicore__ inline Conv3dIntfExt() {}
    };
    struct Conv3dParam : public ConvApi::ConvParam {
        __aicore__ inline Conv3dParam(){};
    };
    ```

    这里的Conv3dIntf是Conv3dIntfExt的基类，Conv3dCfg是Conv3dIntf模板入参，数据结构定义如下：

    ```
    template <class Config, template <typename, class, bool> class Impl>
    struct Conv3dIntf {
        using InputT = typename Config::SrcAT;
        using WeightT = typename Config::SrcBT;
        using OutputT = typename Config::DstT;
        using BiasT = typename Config::BiasT;
        using L0cT = typename Config::L0cT;
        using ConvParam = typename Config::ConvParam;
        __aicore__ inline Conv3dIntf() {}
    } template <class ConvDataType>
    struct Conv3dCfg : public ConvApi::ConvConfig<ConvDataType> {
    public:
        __aicore__ inline Conv3dCfg() {}
        using ContextData = struct _ : public ConvApi::ConvConfig<ConvDataType>::ContextData {
            __aicore__ inline _() {}
        };
    };
    ```

    **表1**  ConvType说明

    <a name="table19081115275"></a>
    | 参数 | 说明 |
    | --- | --- |
    | TPosition | 内存逻辑位置。<br>    Input矩阵可设置为TPosition::GM。<br>Weight矩阵可设置为TPosition::GM。<br>Bias矩阵可设置为TPosition::GM。<br>Output矩阵可设置为TPosition::GM。 |
    | ConvFormat | 数据格式。<br>    Input矩阵可设置为ConvFormat::NDC1HWC0。<br>Weight矩阵可设置为ConvFormat::FRACTAL_Z_3D。<br>Bias矩阵可设置为ConvFormat::ND。<br>Output矩阵可设置为ConvFormat::NDC1HWC0。 |
    | TYPE | 数据类型。<br>Input矩阵可设置为half、bfloat16_t。<br>Weight矩阵可设置为half、bfloat16_t。<br>Bias矩阵可设置为half、float。<br>Output矩阵可设置为half、bfloat16_t。<!-- npu="A3,910b" id6 --><br><br>注意：输入输出的矩阵数据类型需要对应，具体支持的数据类型组合关系请参考表2。<!-- end id6 --> |

    <!-- npu="A3,910b" id5 -->
    **表2**  Conv3D输入输出数据类型的组合说明

    | Input矩阵 | Weight矩阵 | Bias | Output矩阵 | 支持平台 |
    | --- | --- | --- | --- | --- |
    | half | half | half | half | <!-- npu="A3" id1 -->Atlas A3 训练系列产品/Atlas A3 推理系列产品<br><!-- end id1 --><!-- npu="910b" id2 -->Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id2 --> |
    | bfloat16_t | bfloat16_t | float | bfloat16_t | <!-- npu="A3" id3 -->Atlas A3 训练系列产品/Atlas A3 推理系列产品<br><!-- end id3 --><!-- npu="910b" id4 -->Atlas A2 训练系列产品/Atlas A2 推理系列产品<!-- end id4 --> |
    <!-- end id5 -->

2.  初始化操作。

    ```
    Conv3dApi::Conv3D<inputType, weightType, outputType, biasType> conv3dApi;
    TPipe pipe;                                                        // 初始化TPipe
    conv3dApi.Init(&tiling);                                           // 初始化conv3dApi
    ```

3.  设置3D卷积的输入Input、Weight、Bias和输出Output。

    ```
    conv3dApi.SetWeight(weightGm);               // 设置当前核的输入weight在gm上的地址
    if (biasFlag) {
        conv3dApi.SetBias(biasGm);               // 设置当前核的输入bias在gm上的地址
    }
    // 设置input各个维度在当前核的偏移
    conv3dApi.SetInputStartPosition(diStartPos, mStartPos);
    // 设置当前核的cout,dout,m大小
    conv3dApi.SetSingleOutputShape(singleCoreCout, singleCoreDout, singleCoreM);

    // 当前Conv3D仅支持单batch的卷积计算，多batch场景通过for循环实现，在循环间计算当前batch的地址偏移
    for (uint64_t batchIter = 0; batchIter < singleCoreBatch; ++batchIter) {
        conv3dApi.SetInput(inputGm[batchIter * inputOneBatchSize]);    // 设置当前核的输入input在gm上的地址
    }
    ```

4.  完成3D卷积操作。

    调用[IterateAll](IterateAll-103.md)完成单核上所有数据的计算。

    ```
    for (uint64_t batchIter = 0; batchIter < singleCoreBatch; ++batchIter) {
        ...
        conv3dApi.IterateAll(outputGm[batchIter * outputOneBatchSize]);    // 调用IterateAll完成Conv3D计算
        ...
    }
    ```

5.  结束3D卷积操作。

    ```
    for (uint64_t batchIter = 0; batchIter < singleCoreBatch; ++batchIter) {
        ...
        conv3dApi.End();    //清除EventID和释放内部申请的临时内存
    }
    ```

## 需要包含的头文件

```
#include "lib/conv/conv3d/conv3d_api.h"
```

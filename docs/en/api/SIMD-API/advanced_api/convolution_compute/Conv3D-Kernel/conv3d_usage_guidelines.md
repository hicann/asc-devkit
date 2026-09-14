# Conv3D Usage Guidelines

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:06:27.809Z -->

Ascend C provides a set of high-level Conv3D APIs to help you quickly implement 3D forward convolution matrix operations. The schematic diagram of 3D forward convolution is shown in [Figure 1](#fig198021635102613), and its calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002359552057.png)

-   X is the feature matrix **Input** of the Conv3D convolution.

-   W is the weight matrix **Weight** of the Conv3D convolution.

-   B is the bias matrix **Bias** of the Conv3D convolution.

-   Y is the result matrix **Output** after the convolution and bias operations are completed.

**Figure 1** 3D forward convolution diagram<a name="fig198021635102613"></a>  
![](../../../../figures/3D_forward_convolution_diagram.png "3D forward convolution diagram")

> [!NOTE]Description
>Cin is the input channel size **Channel** of **Input**; Din is the **Depth** dimension size of **Input**; Hin is the **Height** dimension size of **Input**; Win is the **Width** dimension size of **Input**; Cout is the output channel size of **Weight** and **Output**; Dout is the **Depth** dimension size of **Output**; Hout is the **Height** dimension size of **Output**; Wout is the **Width** dimension size of **Output**. The M dimension mentioned below is the vertical axis of the input **Input** after img2col expansion during the forward convolution operation, and is numerically equal to Hout \* Wout.
>**Channel**, **Depth**, **Height**, and **Width** are hereinafter abbreviated as C, D, H, and W.

In addition to the basic operations described above, you can set the **Padding**, **Stride**, and **Dilation** parameters in Conv3D computation. Their meanings are as follows.

-   Padding indicates zero-padding applied to the three dimensions of the input matrix. See [Figure 2](#fig16852164019363).

-   Stride indicates the step size of the convolution kernel's movement along the three dimensions. See [Figure 3](#fig137769564018).

-   Dilation indicates the spacing between each element of the convolution kernel along the three dimensions. See [Figure 4](#fig1015315044111).

**Figure 2**  Padding in Conv3D forward computation<a name="fig16852164019363"></a>  
![](../../../../figures/padding_in_conv3d_forward_computation.png "Padding in Conv3D forward computation")

**Figure 3**  Stride in Conv3D forward computation<a name="fig137769564018"></a>  
![](../../../../figures/stride_in_conv3d_forward_computation.png "Stride in Conv3D forward computation")

**Figure 4**  Dilation in Conv3D forward computation<a name="fig1015315044111"></a>  
![](../../../../figures/dilation_in_conv3d_forward_computation.png "Dilation in Conv3D forward computation")

The steps for implementing Conv3D computation on the Kernel side are summarized as follows:
1.  Create a Conv3D object.

2.  Perform initialization.

3.  Set the 3D convolution inputs: `Input`, `Weight`, `Bias`, and `Output`.

4.  Complete the 3D convolution operation.

5.  End the 3D convolution operation.

The specific steps for implementing forward convolution using the Conv3D high-level API are as follows:

1.  Create a Conv3D object.

    ```
    #include "lib/conv/conv3d/conv3d_api.h"

    using inputType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::NDC1HWC0, bfloat16_t>;
    using weightType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::FRACTAL_Z_3D, bfloat16_t>;
    using outputType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::NDC1HWC0, bfloat16_t>;
    using biasType = ConvApi::ConvType<AscendC::TPosition::GM, ConvFormat::ND, float>; // Optional parameter.

    Conv3dApi::Conv3D<inputType, weightType, outputType, biasType> conv3dApi;
    ```

    When creating the object, you need to pass in the type information of the Input, Weight, and Output parameters. The data type of Bias is an optional parameter. For convolution computation scenarios without Bias input, this parameter is not passed in. The type information is defined by ConvType, including the memory logical location, data format, and data type.

    ```
    template <TPosition POSITION, ConvFormat FORMAT, typename TYPE>
    struct ConvType {
        constexpr static TPosition pos = POSITION;    // Location of the Conv3d input or output in memory.
        constexpr static ConvFormat format = FORMAT;  // Data format of the Conv3d input or output.
        using T = TYPE;                               // Data type of the Conv3d input or output.
    };
    ```

    The following briefly introduces the related data structures used when creating the object. You can optionally review this information. The data structures used to create the Conv3D object are defined as follows:

    ```
    template <class INPUT_TYPE, class WEIGHT_TYPE, class OUTPUT_TYPE, class BIAS_TYPE = biasType, class CONV_CFG = Conv3dParam>
    using Conv3D = Conv3dIntfExt<Config<ConvApi::ConvDataType<INPUT_TYPE, WEIGHT_TYPE, OUTPUT_TYPE, BIAS_TYPE, CONV_CFG>>, Impl, Intf>
    ```

    Here, the data structures of **Conv3dIntfExt** and **Conv3dParam** are defined as follows:

    ```
    template <class Conv3dCfg, template <typename, class, bool> class Impl = Conv3dApiImpl,
        template <class, template <typename, class, bool> class> class Intf = Conv3dIntf>
    struct Conv3dIntfExt : public Intf<Conv3dCfg, Impl> {
        __aicore__ inline Conv3dIntfExt()
        {}
    };
    struct Conv3dParam : public ConvApi::ConvParam {
        __aicore__ inline Conv3dParam(){};
    };
    ```

    Here, **Conv3dIntf** is the base class of **Conv3dIntfExt**, and **Conv3dCfg** is the template parameter of **Conv3dIntf**. Their data structures are defined as follows:

    ```
    template <class Config, template <typename, class, bool> class Impl>
    struct Conv3dIntf {
        using InputT = typename Config::SrcAT;
        using WeightT = typename Config::SrcBT;
        using OutputT = typename Config::DstT;
        using BiasT = typename Config::BiasT;
        using L0cT = typename Config::L0cT;
        using ConvParam = typename Config::ConvParam;
        __aicore__ inline Conv3dIntf()
        {}
    }
    template <class ConvDataType>
    struct Conv3dCfg : public ConvApi::ConvConfig<ConvDataType> {
    public:
        __aicore__ inline Conv3dCfg()
        {}
        using ContextData = struct _ : public ConvApi::ConvConfig<ConvDataType>::ContextData {
            __aicore__ inline _()
            {}
        };
    };
    ```

    **Table 1**  ConvType description

    <a name="table19081115275"></a>

    | Parameter | Description |
    | --- | --- |
    | TPosition | Memory logical location.<br>    The Input matrix can be set to **TPosition::GM**.<br>The Weight matrix can be set to **TPosition::GM**.<br>The Bias matrix can be set to **TPosition::GM**.<br>The Output matrix can be set to **TPosition::GM**. |
    | ConvFormat | Data format.<br>    The Input matrix can be set to **ConvFormat::NDC1HWC0**.<br>The Weight matrix can be set to **ConvFormat::FRACTAL_Z_3D**.<br>The Bias matrix can be set to **ConvFormat::ND**.<br>The Output matrix can be set to **ConvFormat::NDC1HWC0**. |
    | TYPE | Data type.<br>The Input matrix can be set to **half** and **bfloat16_t**.<br>The Weight matrix can be set to **half** and **bfloat16_t**.<br>The Bias matrix can be set to **half** and **float**.<br>The Output matrix can be set to **half** and **bfloat16_t**.<br><br>Note: The data types of the input and output matrices must correspond. For the supported data type combinations, see Table 2. |

    **Table 2**  Supported data type combinations for Conv3D input and output

    | Input matrix | Weight matrix | Bias | Output matrix | Supported platforms |
    | --- | --- | --- | --- | --- |
    | half | half | half | half | Atlas A3 training products/Atlas A3 inference products<br>Atlas A2 training products/Atlas A2 inference products |
    | bfloat16_t | bfloat16_t | float | bfloat16_t | Atlas A3 training products/Atlas A3 inference products<br>Atlas A2 training products/Atlas A2 inference products |

2.  Perform initialization.

    ```
    Conv3dApi::Conv3D<inputType, weightType, outputType, biasType> conv3dApi;
    TPipe pipe;                                                        // Initialize TPipe.
    conv3dApi.Init(&tiling);                                           // Initialize conv3dApi.
    ```

3.  Set the input **Input**, **Weight**, **Bias**, and output **Output** of the 3D convolution.

    ```
    conv3dApi.SetWeight(weightGm);               // Set the address of the input weight of the current core in gm.
    if (biasFlag) {
        conv3dApi.SetBias(biasGm);               // Set the address of the input bias of the current core in gm.
    }
    // Set the offset of each dimension of the input on the current core.
    conv3dApi.SetInputStartPosition(diStartPos, mStartPos);
    // Set the cout, dout, and m sizes of the current core.
    conv3dApi.SetSingleOutputShape(singleCoreCout, singleCoreDout, singleCoreM);

    // Currently, Conv3D supports only single-batch convolution computation. Multi-batch scenarios are implemented through a for loop, in which the address offset of the current batch is computed between iterations.
    for (uint64_t batchIter = 0; batchIter < singleCoreBatch; ++batchIter) {
        conv3dApi.SetInput(inputGm[batchIter * inputOneBatchSize]);    // Set the address of the input of the current core in gm.
    }
    ```

4.  Complete the 3D convolution operation.

    Call [IterateAll](IterateAll-103.md) to complete the computation of all data on a single core.

    ```
    for (uint64_t batchIter = 0; batchIter < singleCoreBatch; ++batchIter) {
        ...
        conv3dApi.IterateAll(outputGm[batchIter * outputOneBatchSize]);    // Call IterateAll to complete the Conv3D computation.
        ...
    }
    ```

5.  End the 3D convolution operation.

    ```
    for (uint64_t batchIter = 0; batchIter < singleCoreBatch; ++batchIter) {
        ...
        conv3dApi.End();    //Clear the EventID and release the temporary memory allocated internally.
    }
    ```

## Required Header Files

```
#include "lib/conv/conv3d/conv3d_api.h"
```


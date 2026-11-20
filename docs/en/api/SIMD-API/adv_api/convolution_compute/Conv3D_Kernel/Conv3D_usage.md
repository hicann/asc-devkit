# Conv3D Usage Guide

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-11-10T06:21:32.222Z pushedAt=2026-11-18T11:07:26.497Z nl=2 -->

Ascend C provides a set of Conv3D high-level APIs to help you quickly implement 3D forward convolution matrix operations. The schematic diagram of 3D forward convolution is shown in [Figure 1](#fig198021635102613), and its computation formula is as follows:

![](../../../../figures/formulaimage_0000002359552057.png)

-   X is the feature matrix Input of the Conv3D convolution.
-   W is the weight matrix Weight of the Conv3D convolution.
-   B is the bias matrix Bias of the Conv3D convolution.
-   Y is the result matrix Output after the convolution and bias operations are completed.

**Figure 1**  3D forward convolution diagram<a name="fig198021635102613"></a>  
![](../../../../figures/conv3d_forward_diagram.png "3D forward convolution diagram")

> [!NOTE]Description
>Cin is the input channel size Channel of Input; Din is the Depth dimension size of Input; Hin is the Height dimension size of Input; Win is the Width dimension size of Input; Cout is the output channel size of Weight and Output; Dout is the Depth dimension size of Output; Hout is the Height dimension size of Output; Wout is the Width dimension size of Output; the M dimension mentioned below is the vertical axis of Input after img2col expansion during the forward convolution operation, and is numerically equal to Hout \* Wout.
>Channel, Depth, Height, and Width are hereinafter abbreviated as C, D, H, and W.

In addition to the basic operations described above, the Padding, Stride, and Dilation parameters can be set in Conv3D computation. Their meanings are as follows.

-   Padding indicates zero padding on the three dimensions of the input matrix. See [Figure 2](#fig16852164019363).
-   Stride indicates the sliding distance of the convolution kernel on its three dimensions. See [Figure 3](#fig137769564018).
-   Dilation indicates the spacing between data points on the three dimensions of the convolution kernel. See [Figure 4](#fig1015315044111).

**Figure 2**  Forward 3D convolution padding diagram<a name="fig16852164019363"></a>  
![](../../../../figures/conv3d_padding.png "Forward 3D convolution padding diagram")

**Figure 3**  Forward 3D convolution stride diagram<a name="fig137769564018"></a>  
![](../../../../figures/conv3d_stride.png "Forward 3D convolution stride diagram")

**Figure 4**  Forward 3D convolution dilation diagram<a name="fig1015315044111"></a>  
![](../../../../figures/conv3d_dilation.png "Forward 3D convolution dilation diagram")

The steps for implementing the Conv3D operation on the kernel side are summarized as follows:

1.  Create a Conv3D object.
2.  Initialize the operation.
3.  Set the 3D convolution input Input, Weight, Bias, and output Output.
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

    When creating the object, you need to pass the type information of the Input, Weight, and Output parameters. The Bias parameter type is optional; for convolution computation scenarios without Bias input, this parameter is not passed. The type information is defined by ConvType, including the logical memory position, data format, and data type.

    ```
    template <TPosition POSITION, ConvFormat FORMAT, typename TYPE>
    struct ConvType {
        constexpr static TPosition pos = POSITION;   // Position of the Conv3d input or output in memory.
        constexpr static ConvFormat format = FORMAT; // Data format of the Conv3d input or output.
        using T = TYPE;                              // Data type of the Conv3d input or output.
    };
    ```

    The following briefly introduces the related data structures used when creating the object. You may optionally learn about this content. The data structures used to create the Conv3D object are defined as follows:

    ```
    template <
        class INPUT_TYPE, class WEIGHT_TYPE, class OUTPUT_TYPE, class BIAS_TYPE = biasType, class CONV_CFG = Conv3dParam>
    using Conv3D =
        Conv3dIntfExt<Config<ConvApi::ConvDataType<INPUT_TYPE, WEIGHT_TYPE, OUTPUT_TYPE, BIAS_TYPE, CONV_CFG>>, Impl, Intf>
    ```

    The definitions of the Conv3dIntfExt and Conv3dParam data structures are as follows:

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

    Here, Conv3dIntf is the base class of Conv3dIntfExt, and Conv3dCfg is the template parameter of Conv3dIntf. Their data structure definitions are as follows:

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

    **Table 1**  ConvType description

    <a name="table19081115275"></a>

    | Parameter | Description |
    | --- | --- |
    | TPosition | Logical memory position.<br>    The Input matrix can be set to TPosition::GM.<br>The Weight matrix can be set to TPosition::GM.<br>The Bias matrix can be set to TPosition::GM.<br>The Output matrix can be set to TPosition::GM. |
    | ConvFormat | Data format.<br>    The Input matrix can be set to ConvFormat::NDC1HWC0.<br>The Weight matrix can be set to ConvFormat::FRACTAL_Z_3D.<br>The Bias matrix can be set to ConvFormat::ND.<br>The Output matrix can be set to ConvFormat::NDC1HWC0. |
    | TYPE | Data type.<br>The Input matrix can be set to half or bfloat16_t.<br>The Weight matrix can be set to half or bfloat16_t.<br>The Bias matrix can be set to half or float.<br>The Output matrix can be set to half or bfloat16_t.<!-- npu="A3,910b" id6 --><br><br>Note: The data types of the input and output matrices must correspond to each other. For the supported data type combinations, see Table 2.<!-- end id6 --> |

    <!-- npu="A3,910b" id5 -->
    **Table 2**  Supported data type combinations for Conv3D input and output

    | Input matrix | Weight matrix | Bias | Output matrix | Supported Platform |
    | --- | --- | --- | --- | --- |
    | half | half | half | half | <!-- npu="A3" id1 -->Atlas A3 products<br><!-- end id1 --><!-- npu="910b" id2 -->Atlas A2 products<!-- end id2 --> |
    | bfloat16_t | bfloat16_t | float | bfloat16_t | <!-- npu="A3" id3 -->Atlas A3 products<br><!-- end id3 --><!-- npu="910b" id4 -->Atlas A2 products<!-- end id4 --> |

    <!-- end id5 -->

2.  Initialize the operation.

    ```
    Conv3dApi::Conv3D<inputType, weightType, outputType, biasType> conv3dApi;
    TPipe pipe;                                                        // Initialize TPipe.
    conv3dApi.Init(&tiling);                                           // Initialize conv3dApi.
    ```

3.  Set the input, weight, bias, and output of the 3D convolution.

    ```
    conv3dApi.SetWeight(weightGm);               // Set the GM address of the input weight for the current core.
    if (biasFlag) {
        conv3dApi.SetBias(biasGm);               // Set the GM address of the input bias for the current core.
    }
    // Set the offset of each input dimension on the current core.
    conv3dApi.SetInputStartPosition(diStartPos, mStartPos);
    // Set the cout, dout, and m sizes of the current core.
    conv3dApi.SetSingleOutputShape(singleCoreCout, singleCoreDout, singleCoreM);

    // The current Conv3D supports only single-batch convolution. For multi-batch scenarios, use a for loop and computate the address offset of the current batch between iterations.
    for (uint64_t batchIter = 0; batchIter < singleCoreBatch; ++batchIter) {
        conv3dApi.SetInput(inputGm[batchIter * inputOneBatchSize]);    // Set the GM address of the input for the current core.
    }
    ```

4.  Complete the 3D convolution operation.

    Call [IterateAll](IterateAll.md) to complete the computation of all data on a single core.

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
        conv3dApi.End();    // Clear the event ID and release the internally allocated temporary memory.
    }
    ```

## Header Files to Include

```
#include "lib/conv/conv3d/conv3d_api.h"
```

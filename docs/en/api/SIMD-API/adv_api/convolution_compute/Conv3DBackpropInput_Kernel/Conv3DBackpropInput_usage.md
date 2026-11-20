# Conv3DBackpropInput Usage Guidelines

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-11-10T06:27:52.526Z pushedAt=2026-11-18T11:07:26.669Z nl=2 -->

Ascend C provides a set of Conv3DBackpropInput high-order APIs to help you quickly implement the backward operation of convolution and solve the backpropagation error. Transposed convolution Conv3DTranspose and Conv3DBackpropInput share the same mathematical process, so you can also use the Conv3DBackpropInput high-order APIs to implement the transposed convolution operator. The forward and backward propagation of convolution is shown in [Figure 1 Forward and backward propagation of the convolutional layer](#fig1069918872512), and the backpropagation error computation is shown in [Figure 2 Backpropagation error computation](#fig1953483815252).

The computation formula of Conv3DBackpropInput is:

![](../../../../figures/formulaimage_0000002073192576.png)

-   ∂L/∂Y is the gradient GradOutput of the convolution forward loss function with respect to the output Y, which serves as the input for solving the backpropagation error ∂L/∂X.
-   W is the convolution forward Weight, that is, the matrix kernel-side API, which is also the filter Filter. It serves as the input for solving the backpropagation error ∂L/∂X, and W<sup>T</sup> represents the transpose of W.
-   ∂L/∂X is the backpropagation error GradInput of the feature matrix.

**Figure 1**  Forward and backward propagation of the convolutional layer<a name="fig1069918872512"></a>  
![](../../../../figures/conv_forward_backward.png "Forward and backward propagation of the convolutional layer")

**Figure 2**  Backpropagation error computation<a name="fig1953483815252"></a>  
![](../../../../figures/backward_error_compute.png "Backpropagation error computation")

The steps for implementing the Conv3DBackpropInput operation to solve the backpropagation error on the kernel-side are summarized as follows:

1.  Create a Conv3DBackpropInput object.
2.  Initialize the operation.
3.  Set GradOutput, the output of the convolution backward operation, and Weight, the input of the convolution.
4.  Complete the convolution backward operation.
5.  End the convolution backward operation.

    > [!NOTE]
    > In the following description, the M axis direction is the vertical direction of the GradOutput matrix; the K axis direction is the horizontal direction of the GradOutput matrix or the vertical direction of the Weight matrix; and the N axis direction is the horizontal direction of the Weight matrix.

The specific steps for using the high-order Conv3DBackpropInput API to solve the backpropagation error computation are as follows:

1.  Create a Conv3DBackpropInput object.

    ```
    #include "lib/conv_backprop/conv3d_bp_input_api.h"

    using weightDxType =
        ConvBackpropApi::ConvType<ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::FRACTAL_Z_3D, weightType>;
    using inputSizeDxType = ConvBackpropApi::ConvType<ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::ND, int32_t>;
    using gradOutputDxType =
        ConvBackpropApi::ConvType<ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::NDC1HWC0, gradOutputType>;
    using gradInputDxType =
        ConvBackpropApi::ConvType<ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::NCDHW, gradInputType>;
    ConvBackpropApi::Conv3DBackpropInput<weightDxType, inputSizeDxType, gradOutputDxType, gradInputDxType> gradInput_;
    ```

    When creating the object, you need to  the weight matrix Weight, the shape information InputSize of the convolution forward feature matrix Input, and the parameter type information of GradOutput and GradInput. The type information is defined by [ConvType](#table19081115275), including the memory logical position, data format, and data type.

    ```
    template <TPosition POSITION, ConvFormat FORMAT, typename T>
    struct ConvType {
        constexpr static TPosition pos = POSITION;   // Logical position of the Convolution input or output.
        constexpr static ConvFormat format = FORMAT; // Data format of the Convolution input or output.
        using Type = T;                              // Data type of the Convolution input or output.
    };
    ```

    The following briefly describes the data structures used when creating an object. You can read this section as needed. The data structures used to create a Conv3DBackpropInput object are defined as follows:

    ```
    using Conv3DBackpropInput = Conv3DBpInputIntf<
        Conv3DBpInputCfg<WEIGHT_TYPE, INPUT_TYPE, GRAD_OUTPUT_TYPE, GRAD_INPUT_TYPE, CONV3D_CFG_DEFAULT>,
        Conv3DBpInputImpl>;
    ```

    The Conv3DBpInputIntf and Conv3DBpInputCfg data structures are defined as follows:

    ```
    template <class Config_, template <typename, class> class Impl>
    struct Conv3DBpInputIntf {}
    ```

    ```
    template <
        class WEIGHT_TYPE, class INPUT_TYPE, class GRAD_OUTPUT_TYPE, class GRAD_INPUT_TYPE,
        const Conv3dConfig& CONV3D_CONFIG = CONV3D_CFG_DEFAULT>
    struct Conv3DBpInputCfg : public ConvBpContext<WEIGHT_TYPE, INPUT_TYPE, GRAD_OUTPUT_TYPE, GRAD_INPUT_TYPE> {}
    ```

    **Table 1**  ConvType description

    <a name="table19081115275"></a>

    | Parameter | Description |
    | --- | --- |
    | POSITION | Logical memory position.<br>    The Weight matrix can be set to TPosition::GM.<br>The GradOutput matrix can be set to TPosition::GM.<br>InputSize can be set to TPosition::GM.<br>The GradInput matrix can be set to TPosition::GM. |
    | ConvFormat | Data format.<br>    The Weight matrix can be set to ConvFormat::FRACTAL_Z_3D.<br>The GradOutput matrix can be set to ConvFormat::NDC1HWC0.<br>The InputSize matrix can be set to ConvFormat::ND.<br>The GradInput matrix can be set to ConvFormat::NDC1HWC0. |
    | TYPE | Data type.<br>The Weight matrix can be set to half or bfloat16_t.<br>The GradOutput matrix can be set to half or bfloat16_t.<br>The InputSize matrix can be set to int32_t.<br>The GradInput matrix can be set to half or bfloat16_t.<!-- npu="A3,910b" id6 --><br><br>Note: The data types of the GradOutput matrix and the Weight matrix must be the same. For the supported data type combinations, see Table 2.<!-- end id6 --> |

    <!-- npu="A3,910b" id5 -->
    **Table 2**  Supported data type combinations for Conv3DBackpropInput input and output

    | Weight | GradOutput | InputSize | GradInput | Supported Platform |
    | --- | --- | --- | --- | --- |
    | half | half | int32_t | half | <!-- npu="A3" id1 -->Atlas A3 products<br><!-- end id1 --><!-- npu="910b" id2 -->Atlas A2 products<!-- end id2 --> |
    | bfloat16_t | bfloat16_t | int32_t | bfloat16_t | <!-- npu="A3" id3 -->Atlas A3 products<br><!-- end id3 --><!-- npu="910b" id4 -->Atlas A2 products<!-- end id4 --> |

    <!-- end id5 -->

2.  Initialize the operation.

    ```
    // Initialize after registration.
    ConvBackpropApi::Conv3DBackpropInput<weightDxType, inputSizeDxType, gradOutputDxType, gradInputDxType> gradInput_;
    gradInput_.Init(&(tilingData->conv3DDxTiling));
    ```

3.  Set the output GradOutput of the 3D convolution backward operation and the input Weight of the 3D convolution.

    ```
    gradInput_.SetSingleShape(singleShapeM_, singleShapeK_, singleShapeN_); // Set the shape for single-core computation.
    gradInput_.SetStartPosition(dinStartIdx_, curHoStartIdx_); // Set the start position for loading gradOutput on a single core.
    gradInput_.SetGradOutput(gradOutputGm_[offsetA_]);
    gradInput_.SetWeight(weightGm_[offsetB_]);
    ```

4.  Complete the convolution backward operation.

    Call [Iterate](Iterate.md) to complete a single iteration, and use a while loop to complete the computation of all data on a single core. With the Iterate method, you can control the number of iterations to compute the required amount of data.

    ```
    while (gradInput_.Iterate()) {
        gradInput_.GetTensorC(gradInputGm_[offsetC_]);
    }
    ```

5.  End the convolution backward operation.

    ```
    gradInput_.End();
    ```

## Header Files to Include

```
#include "lib/conv_backprop/conv3d_bp_input_api.h"
```
